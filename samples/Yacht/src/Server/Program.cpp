// Program.cpp -- C++ port of Server/Program.cs.
//
// The game server: a console program that hosts YachtService until someone presses enter.

#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "System/Convert.hpp"
#include "System/Guid.hpp"
#include "System/ServiceModel/EndpointAddress.hpp"
#include "System/ServiceModel/ServiceHost.hpp"
#include "System/Xml/XmlNodeType.hpp"
#include "System/Xml/XmlReader.hpp"

#include "../YachtServices/YachtService.hpp"

namespace {

using System::ServiceModel::EndpointAddress;
using System::ServiceModel::ServiceHost;
using System::Xml::XmlNodeType;
using System::Xml::XmlReader;

// The address the original's App.config names as the host's base address.
constexpr const char* kBaseAddress = "http://localhost:8888/GameServer/";
constexpr const char* kContractNamespace = "http://tempuri.org/";
constexpr const char* kContractName = "IYachtService";

// Reads one named parameter out of the operation element the host handed over. The host cannot
// do this itself: a wrapped SOAP call carries no type information, so which elements to expect
// is the contract's knowledge, and the contract is this file's.
std::string ReadParameter(XmlReader& reader, const std::string& name)
{
    while (reader.Read()) {
        if (reader.getNodeTypeProperty() == XmlNodeType::Element &&
            reader.getNameProperty() == name) {
            if (!reader.Read()) {
                return {};
            }
            return reader.getValueProperty();
        }
    }
    return {};
}

std::string Base64(const std::vector<SharpRuntime::bytecs>& bytes)
{
    return bytes.empty() ? std::string() : System::Convert::ToBase64String(bytes);
}

void RegisterOperations(ServiceHost& host, YachtServices::YachtService& service)
{
    host.AddOperation("Register", [&service](XmlReader& p) -> std::optional<std::string> {
        const std::string clientURI = ReadParameter(p, "clientURI");
        const std::string name = ReadParameter(p, "name");
        const std::string playerID = ReadParameter(p, "playerID");
        return std::to_string(
            service.Register(System::Uri(clientURI), name, std::stoi(playerID)));
    });

    host.AddOperation("Unregister", [&service](XmlReader& p) -> std::optional<std::string> {
        service.Unregister(std::stoi(ReadParameter(p, "sessionID")));
        return std::nullopt;
    });

    host.AddOperation("JoinGame", [&service](XmlReader& p) -> std::optional<std::string> {
        const std::string gameID = ReadParameter(p, "gameID");
        const std::string sessionID = ReadParameter(p, "sessionID");
        return service.JoinGame(System::Guid(gameID), std::stoi(sessionID)) ? "true" : "false";
    });

    host.AddOperation("LeaveGame", [&service](XmlReader& p) -> std::optional<std::string> {
        return service.LeaveGame(std::stoi(ReadParameter(p, "sessionID"))) ? "true" : "false";
    });

    host.AddOperation("GameStep", [&service](XmlReader& p) -> std::optional<std::string> {
        const std::string gameID = ReadParameter(p, "gameID");
        const std::string sessionID = ReadParameter(p, "sessionID");
        const std::string scoreLine = ReadParameter(p, "scoreLine");
        const std::string score = ReadParameter(p, "score");
        const std::string player = ReadParameter(p, "player");
        const std::string step = ReadParameter(p, "step");
        service.GameStep(System::Guid(gameID), std::stoi(sessionID), std::stoi(scoreLine),
                         static_cast<SharpRuntime::bytecs>(std::stoi(score)), std::stoi(player),
                         std::stoi(step));
        return std::nullopt;
    });

    host.AddOperation("GetGameState", [&service](XmlReader& p) -> std::optional<std::string> {
        const std::string gameID = ReadParameter(p, "gameID");
        const std::string sessionID = ReadParameter(p, "sessionID");
        return Base64(service.GetGameState(System::Guid(gameID), std::stoi(sessionID)));
    });

    host.AddOperation("GetAvailableGames",
                      [&service](XmlReader& p) -> std::optional<std::string> {
                          return Base64(service.GetAvailableGames(
                              std::stoi(ReadParameter(p, "sessionID"))));
                      });

    host.AddOperation("NewGame", [&service](XmlReader& p) -> std::optional<std::string> {
        const std::string sessionID = ReadParameter(p, "sessionID");
        const std::string name = ReadParameter(p, "name");
        return Base64(service.NewGame(std::stoi(sessionID), name));
    });

    host.AddOperation("ResetTimeout", [&service](XmlReader& p) -> std::optional<std::string> {
        const std::string gameID = ReadParameter(p, "gameID");
        const std::string sessionID = ReadParameter(p, "sessionID");
        service.ResetTimeout(System::Guid(gameID), std::stoi(sessionID));
        return std::nullopt;
    });

    host.AddOperation("GetScoreCard", [&service](XmlReader& p) -> std::optional<std::string> {
        return Base64(service.GetScoreCard(std::stoi(ReadParameter(p, "sessionID"))));
    });
}

} // namespace

int main()
{
    YachtServices::YachtService service;
    ServiceHost host(EndpointAddress(kBaseAddress), kContractNamespace, kContractName);

    RegisterOperations(host, service);
    host.Open();

    std::cout << "Yacht Service is up..." << std::endl;
    std::cout << "Press enter to close the service" << std::endl;

    std::string line;
    std::getline(std::cin, line);

    host.Close();
    std::cout << "service is closed" << std::endl;
    return 0;
}
