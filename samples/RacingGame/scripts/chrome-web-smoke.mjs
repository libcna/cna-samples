// SPDX-License-Identifier: MS-PL

import fs from "node:fs";

const endpoint = process.env.CNA_CHROME_ENDPOINT ?? "http://127.0.0.1:19522";
const targetUrl = process.env.CNA_RACING_URL;
const writeStorageToken = process.env.CNA_WRITE_STORAGE_TOKEN;
const expectedStorageToken = process.env.CNA_EXPECT_STORAGE_TOKEN;
const evidence = process.argv[2];
if (!evidence) throw new Error("evidence directory required");
fs.mkdirSync(evidence, {recursive: true});

const pages = await (await fetch(`${endpoint}/json/list`)).json();
const page = pages.find((item) => item.type === "page" &&
    (item.url.includes("RacingGame_cna_samples.html") || targetUrl));
if (!page) throw new Error("Racing Game page not found");

const socket = new WebSocket(page.webSocketDebuggerUrl);
const pending = new Map();
const consoleMessages = [];
const consoleDetails = [];
const exceptions = [];
const exceptionDetails = [];
const httpErrors = [];
const networkResources = [];
let nextId = 1;

socket.addEventListener("message", (event) => {
    const message = JSON.parse(event.data);
    if (message.method === "Target.attachedToTarget") {
        send("Runtime.enable", {}, message.params.sessionId).catch((error) =>
            consoleMessages.push(`CDP worker Runtime.enable failed: ${error}`));
    }
    if (message.method === "Runtime.consoleAPICalled") {
        consoleMessages.push(message.params.args
            .map((argument) => argument.value ?? argument.description ?? "")
            .join(" "));
        consoleDetails.push({...message.params, cdpSessionId: message.sessionId ?? null});
    } else if (message.method === "Runtime.exceptionThrown") {
        const details = message.params.exceptionDetails;
        exceptions.push(details.exception?.description ?? details.text);
        exceptionDetails.push({...details, cdpSessionId: message.sessionId ?? null});
    } else if (message.method === "Network.responseReceived") {
        const response = message.params.response;
        if (/RacingGame(?:_cna_samples|content-[^.]+)\.(?:data|wasm|js)$/.test(
                response.url)) {
            networkResources.push({
                name: response.url.split("/").pop(),
                status: response.status,
                mimeType: response.mimeType,
                fromDiskCache: response.fromDiskCache,
                fromServiceWorker: response.fromServiceWorker,
            });
        }
        if (response.status >= 400 &&
            !response.url.endsWith("/favicon.ico"))
            httpErrors.push(`${response.status} ${response.url}`);
    }
    if (!message.id || !pending.has(message.id)) return;
    const {resolve, reject, timer} = pending.get(message.id);
    pending.delete(message.id);
    clearTimeout(timer);
    if (message.error) reject(new Error(JSON.stringify(message.error)));
    else resolve(message.result);
});

await new Promise((resolve, reject) => {
    socket.addEventListener("open", resolve, {once: true});
    socket.addEventListener("error", reject, {once: true});
});

function send(method, params = {}, sessionId = undefined) {
    const id = nextId++;
    socket.send(JSON.stringify({id, method, params,
        ...(sessionId ? {sessionId} : {})}));
    return new Promise((resolve, reject) => {
        const timer = setTimeout(() => {
            pending.delete(id);
            reject(new Error(`${method} timed out`));
        }, 60000);
        pending.set(id, {resolve, reject, timer});
    });
}

const sleep = (milliseconds) =>
    new Promise((resolve) => setTimeout(resolve, milliseconds));

async function evaluate(expression) {
    const answer = await send("Runtime.evaluate", {
        expression, returnByValue: true, awaitPromise: true,
    });
    if (answer.exceptionDetails)
        throw new Error(answer.exceptionDetails.exception?.description ??
                        answer.exceptionDetails.text);
    return answer.result.value;
}

async function waitFor(predicate, description, timeoutMilliseconds) {
    const deadline = Date.now() + timeoutMilliseconds;
    let lastValue;
    while (Date.now() < deadline) {
        try {
            lastValue = await evaluate(predicate);
            if (lastValue) return lastValue;
        } catch (_) {
            // A reload briefly invalidates the execution context.
        }
        await sleep(250);
    }
    const diagnostics = await evaluate(`JSON.stringify({
        packages: globalThis.Module?.racingContentPackages ?? null,
        console: globalThis.__sample152Console?.slice(-20) ?? []
    })`);
    throw new Error(`timed out waiting for ${description}; last value: ${lastValue}; ` +
        `diagnostics: ${diagnostics}`);
}

async function key(type, name, code, virtualKeyCode) {
    await send("Input.dispatchKeyEvent", {
        type, key: name, code,
        windowsVirtualKeyCode: virtualKeyCode,
        nativeVirtualKeyCode: virtualKeyCode,
    });
}

async function tapKey(name, code, virtualKeyCode) {
    await key("rawKeyDown", name, code, virtualKeyCode);
    await Promise.all([sleep(750), waitAnimationFrames(4)]);
    await key("keyUp", name, code, virtualKeyCode);
    // A wall-clock delay is not sufficient for this sample under SwiftShader:
    // a content-heavy frame can occupy the main thread for several seconds.
    // Observe several browser frames in each state so XNA's edge detector sees
    // both the press and the release before the next press.
    await Promise.all([sleep(750), waitAnimationFrames(4)]);
}

async function waitAnimationFrames(count, timeoutMilliseconds = 30000) {
    const start = await evaluate("window.__sample152AnimationFrames");
    return waitFor(
        `window.__sample152AnimationFrames >= ${start + count}`,
        `${count} browser animation frames`, timeoutMilliseconds);
}

async function clickElement(selector) {
    const rectangle = await evaluate(`(() => {
        const rectangle = document.querySelector(${JSON.stringify(selector)})
            .getBoundingClientRect();
        return {x: rectangle.x + rectangle.width / 2,
            y: rectangle.y + rectangle.height / 2};
    })()`);
    await send("Input.dispatchMouseEvent", {
        type: "mousePressed", x: rectangle.x, y: rectangle.y,
        button: "left", clickCount: 1,
    });
    await send("Input.dispatchMouseEvent", {
        type: "mouseReleased", x: rectangle.x, y: rectangle.y,
        button: "left", clickCount: 1,
    });
}

await send("Runtime.enable");
await send("Page.enable");
await send("Network.enable");
if (process.env.CNA_CLEAR_SITE_DATA === "1") {
    await send("Storage.clearDataForOrigin", {
        origin: new URL(targetUrl ?? page.url).origin,
        storageTypes: "all",
    });
}
await send("Target.setAutoAttach", {
    autoAttach: true,
    waitForDebuggerOnStart: false,
    flatten: true,
});
await send("Page.addScriptToEvaluateOnNewDocument", {
    source: `window.__sample152Rejections = [];
        window.__sample152Errors = [];
        window.__sample152ContextEvents = [];
        window.__sample152Keys = [];
        window.__sample152Console = [];
        window.__sample152AnimationFrames = 0;
        for (const name of ['log', 'warn', 'error']) {
            const original = console[name].bind(console);
            console[name] = (...values) => {
                window.__sample152Console.push(name + ': ' + values.map(String).join(' '));
                original(...values);
            };
        }
        window.addEventListener('unhandledrejection', event =>
            window.__sample152Rejections.push(String(event.reason)));
        window.addEventListener('error', event =>
            window.__sample152Errors.push(String(event.error ?? event.message)));
        window.addEventListener('webglcontextlost', () =>
            window.__sample152ContextEvents.push('lost'), true);
        window.addEventListener('webglcontextrestored', () =>
            window.__sample152ContextEvents.push('restored'), true);
        for (const type of ['keydown', 'keyup']) {
            window.addEventListener(type, event =>
                window.__sample152Keys.push(type + ' ' + event.code), true);
        }
        (function countFrame() {
            ++window.__sample152AnimationFrames;
            requestAnimationFrame(countFrame);
        })();`,
});

const loadStart = Date.now();
if (targetUrl)
    await send("Page.navigate", {url: targetUrl});
else
    await send("Page.reload", {ignoreCache: true});
await waitFor(`(() => {
    const button = document.querySelector('#start-button');
    return Module.racingStorage === 'ready' && button &&
        !button.disabled && !button.closest('[hidden]');
})()`, "persistent storage and audio-unlock button", 180000);
const runtimeReadyMilliseconds = Date.now() - loadStart;
await clickElement("#start-button");
await waitFor(`(() => {
    const canvas = document.querySelector('#canvas');
    const status = document.querySelector('#status')?.textContent ?? '';
    const rectangle = canvas?.getBoundingClientRect();
    return document.title === 'Racing Game' && Module.racingGameStarted && canvas &&
        canvas.width >= 640 && canvas.height >= 480 &&
        rectangle.width > 0 && rectangle.height > 0 &&
        !status.startsWith('Downloading') && !status.startsWith('Preparing');
})()`, "Racing Game startup", 180000);
const startupMilliseconds = Date.now() - loadStart;
await sleep(3000);
await send("Page.bringToFront");
await evaluate("document.querySelector('#canvas').focus()");
// LoadContent advances Models, Landscape, Textures, Ready and Complete on
// separate frames. Do not send the splash input while those stages still own
// the screen; SwiftShader makes the individual stages much slower than on GPU.
await waitAnimationFrames(12, 120000);

async function canvasRectangle() {
    return evaluate(`(() => {
        const rectangle = document.querySelector('#canvas').getBoundingClientRect();
        return rectangle.width > 0 && rectangle.height > 0 &&
            {x: rectangle.x, y: rectangle.y,
            width: rectangle.width, height: rectangle.height, scale: 1};
    })()`);
}

async function capture(name) {
    // A screenshot request can meet the compositor between the two triangles
    // of a full-screen post-process draw. Sample several completed animation
    // intervals and keep the richest PNG; this preserves the production
    // preserveDrawingBuffer=false context while avoiding a torn evidence frame.
    let best = Buffer.alloc(0);
    for (let attempt = 0; attempt < 4; ++attempt) {
        await waitAnimationFrames(1);
        const rect = await canvasRectangle();
        const result = await send("Page.captureScreenshot", {
            format: "png", ...(rect ? {clip: rect} : {}), fromSurface: true,
        });
        const bytes = Buffer.from(result.data, "base64");
        if (bytes.length > best.length) best = bytes;
    }
    fs.writeFileSync(`${evidence}/${name}.png`, best);
}

await capture("web-progressive-loading");
const progressiveStatus = await waitFor(`(() => {
    const packages = Module.racingContentPackages;
    if (packages && ['models', 'landscape', 'textures']
        .every(name => packages[name] === 'ready')) return 'ready';
    const canvas = document.querySelector('#canvas');
    if (packages?.models === 'ready' && canvas.width === 0) return 'terminated';
    return false;
})()`, "all progressive Racing content groups", 240000);
const contentReadyMilliseconds = Date.now() - loadStart;
if (progressiveStatus !== "ready") {
    const diagnostics = await evaluate(`JSON.stringify({
        packages: Module.racingContentPackages,
        console: window.__sample152Console.slice(-20)
    })`);
    throw new Error(`Racing stopped during progressive loading: ${diagnostics}`);
}
const storageProbePath = "/save/RacingGame/RacingGame/BrowserStorageProbe.txt";
const settingsPath =
    "/save/RacingGame/RacingGame/AllPlayers/RacingGameSettings.xml";
if (expectedStorageToken) {
    const stored = JSON.parse(await evaluate(`JSON.stringify((() => {
        try {
            return {
                token: FS.readFile(${JSON.stringify(storageProbePath)},
                    {encoding: 'utf8'}),
                settings: FS.readFile(${JSON.stringify(settingsPath)},
                    {encoding: 'utf8'}),
            };
        } catch (_) {
            return null;
        }
    })())`));
    if (stored?.token !== expectedStorageToken ||
        !stored.settings.includes("<GameSettings"))
        throw new Error(`persistent Racing storage mismatch: ${JSON.stringify(stored)}`);
}
if (writeStorageToken) {
    await evaluate(`new Promise((resolve, reject) => {
        try {
            FS.mkdirTree('/save/RacingGame/RacingGame');
            FS.writeFile(${JSON.stringify(storageProbePath)},
                ${JSON.stringify(writeStorageToken)});
            FS.syncfs(false, error => error ? reject(error) : resolve(true));
        } catch (error) {
            reject(error);
        }
    })`);
}
await Promise.all([sleep(1500), waitAnimationFrames(30, 120000)]);
await capture("web-splash");
await tapKey("Space", "Space", 32);
await capture("web-main-menu");
if (writeStorageToken) {
    await tapKey("ArrowRight", "ArrowRight", 39);
    await tapKey("ArrowRight", "ArrowRight", 39);
    await tapKey("Space", "Space", 32);
    await capture("web-options-storage");
    await tapKey("Escape", "Escape", 27);
    await tapKey("ArrowLeft", "ArrowLeft", 37);
    await tapKey("ArrowLeft", "ArrowLeft", 37);
    await evaluate(`new Promise((resolve, reject) => FS.syncfs(false,
        error => error ? reject(error) : resolve(true)))`);
    const settingsWritten = await evaluate(`(() => {
        try {
            return FS.readFile(${JSON.stringify(settingsPath)},
                {encoding: 'utf8'}).includes('<GameSettings');
        } catch (_) {
            return false;
        }
    })()`);
    if (!settingsWritten)
        throw new Error("Options did not persist RacingGameSettings.xml");
}
await tapKey("Space", "Space", 32);
await capture("web-car-selection");
await tapKey("Space", "Space", 32);
await capture("web-track-selection");
await tapKey("Space", "Space", 32);
await waitAnimationFrames(16, 120000);
await capture("web-race-rest");

await key("rawKeyDown", "w", "KeyW", 87);
await key("rawKeyDown", "ArrowLeft", "ArrowLeft", 37);
await waitAnimationFrames(8, 120000);
await key("keyUp", "ArrowLeft", "ArrowLeft", 37);
await key("keyUp", "w", "KeyW", 87);
await waitAnimationFrames(2);
await capture("web-race-driven");

await tapKey("Escape", "Escape", 27);
await capture("web-race-exit");

const browserState = await evaluate(`(() => {
    const canvas = document.querySelector('#canvas');
    const gl = canvas.getContext('webgl2');
    const resources = performance.getEntriesByType('resource')
        .filter(entry => /RacingGame(?:_cna_samples|content-[^.]+)\\.(data|wasm|js)$/.test(entry.name))
        .map(entry => ({name: entry.name.split('/').pop(),
            duration: entry.duration, transferSize: entry.transferSize,
            decodedBodySize: entry.decodedBodySize}));
    return {
        title: document.title,
        status: document.querySelector('#status')?.textContent ?? '',
        crossOriginIsolated: window.crossOriginIsolated,
        canvas: {width: canvas.width, height: canvas.height},
        drawingBuffer: gl ? {width: gl.drawingBufferWidth,
            height: gl.drawingBufferHeight} : null,
        renderer: gl ? gl.getParameter(gl.RENDERER) : null,
        animationFrames: window.__sample152AnimationFrames,
        rejections: window.__sample152Rejections,
        windowErrors: window.__sample152Errors,
        contextEvents: window.__sample152ContextEvents,
        keyEvents: window.__sample152Keys,
        audioUnlocked: Module.racingAudioUnlocked === true,
        audioContextState: Module.SDL3?.audioContext?.state ?? null,
        audioSampleRate: Module.SDL3?.audioContext?.sampleRate ?? null,
        audioPlaybackConnected:
            Boolean(Module.SDL3?.audio_playback?.scriptProcessorNode),
        storageState: Module.racingStorage ?? null,
        storageProbeToken: (() => {
            try {
                return FS.readFile(${JSON.stringify(storageProbePath)},
                    {encoding: 'utf8'});
            } catch (_) {
                return null;
            }
        })(),
        settingsXmlPresent: (() => {
            try {
                return FS.readFile(${JSON.stringify(settingsPath)},
                    {encoding: 'utf8'}).includes('<GameSettings');
            } catch (_) {
                return false;
            }
        })(),
        contentPackages: Module.racingContentPackages ?? null,
        preloadResults: Object.fromEntries(Object.entries(
            Module.preloadResults ?? {}).map(([name, value]) =>
                [name.split('/').pop(), value])),
        resources,
    };
})()`);

const webGlErrors = consoleMessages.filter((message) =>
    /WebGL.*(?:INVALID_|error)|GL_INVALID_|render loop error/i.test(message));
const result = {
    runtimeReadyMilliseconds,
    startupMilliseconds,
    contentReadyMilliseconds,
    loadMilliseconds: Date.now() - loadStart,
    ...browserState,
    exceptions,
    httpErrors,
    webGlErrors,
    networkResources,
};
fs.writeFileSync(`${evidence}/console.log`, consoleMessages.join("\n") + "\n");
fs.writeFileSync(`${evidence}/console-details.json`,
    JSON.stringify(consoleDetails, null, 2) + "\n");
fs.writeFileSync(`${evidence}/exception-details.json`,
    JSON.stringify(exceptionDetails, null, 2) + "\n");
fs.writeFileSync(`${evidence}/result.json`,
    JSON.stringify(result, null, 2) + "\n");

if (!result.crossOriginIsolated)
    throw new Error("threaded Wasm page is not cross-origin isolated");
if (!result.drawingBuffer)
    throw new Error("Racing Game did not obtain a WebGL 2 context");
if (!result.audioUnlocked)
    throw new Error("Racing Game did not start from the browser audio-unlock gesture");
if (result.audioContextState !== "running" || !result.audioPlaybackConnected ||
    result.audioSampleRate <= 0)
    throw new Error(`Racing WebAudio output is not running: ${JSON.stringify({
        state: result.audioContextState,
        sampleRate: result.audioSampleRate,
        connected: result.audioPlaybackConnected,
    })}`);
if (result.storageState !== "ready")
    throw new Error(`Racing IDBFS state is ${result.storageState}`);
if ((writeStorageToken || expectedStorageToken) && !result.settingsXmlPresent)
    throw new Error("RacingGameSettings.xml is missing from persistent storage");
if (!result.contentPackages ||
    ["models", "landscape", "textures"].some(
        name => result.contentPackages[name] !== "ready"))
    throw new Error(`progressive content was incomplete: ${JSON.stringify(
        result.contentPackages)}`);
if (result.animationFrames < 60)
    throw new Error(`expected at least 60 browser frames, got ${result.animationFrames}`);
if (result.keyEvents.filter(event => event === "keydown Space").length < 4 ||
    !result.keyEvents.includes("keydown KeyW") ||
    !result.keyEvents.includes("keydown ArrowLeft"))
    throw new Error(`browser input sequence was incomplete: ${result.keyEvents}`);
if (result.rejections.length || result.windowErrors.length ||
    result.exceptions.length || result.httpErrors.length || result.webGlErrors.length) {
    throw new Error(JSON.stringify(result));
}
if (process.env.CNA_EXPECT_CLEAN_TRANSFER === "1") {
    for (const name of [
        "RacingGame_cna_samples.data",
        "RacingGame-content-models.data",
        "RacingGame-content-landscape.data",
        "RacingGame-content-textures.data",
    ]) {
        if (result.preloadResults?.[name]?.fromCache !== false)
            throw new Error(`clean profile did not download ${name}: ${JSON.stringify(
                result.preloadResults?.[name])}`);
    }
}
if (process.env.CNA_EXPECT_CACHED_CONTENT === "1") {
    for (const name of [
        "RacingGame_cna_samples.data",
        "RacingGame-content-models.data",
        "RacingGame-content-landscape.data",
        "RacingGame-content-textures.data",
    ]) {
        if (result.preloadResults?.[name]?.fromCache !== true)
            throw new Error(`cached profile did not reuse ${name}: ${JSON.stringify(
                result.preloadResults?.[name])}`);
    }
}

console.log(JSON.stringify(result, null, 2));
socket.close();
