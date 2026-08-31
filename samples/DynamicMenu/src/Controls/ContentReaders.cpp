// SPDX-License-Identifier: MS-PL
#include "ContentReaders.hpp"

#include <memory>
#include <string>
#include <vector>

#include "Button.hpp"
#include "Container.hpp"
#include "Control.hpp"
#include "IControl.hpp"
#include "Image.hpp"
#include "Label.hpp"
#include "MultilineTextControl.hpp"
#include "ProgressBar.hpp"
#include "TextControl.hpp"
#include "CNA/Internal/Xnb/CollectionContentTypeReaders.hpp"
#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReaderManager.hpp"
#include "Microsoft/Xna/Framework/Content/ReflectiveTypeReader.hpp"

namespace DynamicMenu::Controls
{
    using Microsoft::Xna::Framework::Content::ContentReader;
    using Microsoft::Xna::Framework::Content::ContentTypeReaderBase;
    using Microsoft::Xna::Framework::Content::ContentTypeReaderManager;
    using Microsoft::Xna::Framework::Content::ReflectiveTypeReader;
    using Microsoft::Xna::Framework::Content::ReflectiveTypeReaderBuilder;

    namespace
    {
        ReflectiveTypeReaderBuilder<Control> DescribeControl()
        {
            return ReflectiveTypeReaderBuilder<Control>("DynamicMenu.Controls.Control")
                .Custom([](Control& value, ContentReader& input) {
                    value.setLeftProperty(input.ReadInt32());
                })
                .Custom([](Control& value, ContentReader& input) {
                    value.setTopProperty(input.ReadInt32());
                })
                .Custom([](Control& value, ContentReader& input) {
                    value.setWidthProperty(input.ReadInt32());
                })
                .Custom([](Control& value, ContentReader& input) {
                    value.setHeightProperty(input.ReadInt32());
                })
                .Custom([](Control& value, ContentReader& input) {
                    value.setNameProperty(input.ReadObject<std::string>());
                })
                .Custom([](Control& value, ContentReader& input) {
                    value.setBackTextureNameProperty(input.ReadObject<std::string>());
                })
                .Custom([](Control& value, ContentReader& input) {
                    value.setVisibleProperty(input.ReadBoolean());
                })
                .Custom([](Control& value, ContentReader& input) {
                    value.setHueProperty(input.ReadColor());
                });
        }

        ReflectiveTypeReaderBuilder<TextControl> DescribeTextControl(
            const ReflectiveTypeReaderBuilder<Control>& control)
        {
            return ReflectiveTypeReaderBuilder<TextControl>("DynamicMenu.Controls.TextControl")
                .Base(control)
                .Custom([](TextControl& value, ContentReader& input) {
                    value.setTextProperty(input.ReadObject<std::string>());
                })
                .Custom([](TextControl& value, ContentReader& input) {
                    value.setFontNameProperty(input.ReadObject<std::string>());
                })
                .Custom([](TextControl& value, ContentReader& input) {
                    value.setTextColorProperty(input.ReadColor());
                });
        }
    }

    void RegisterDynamicMenuContentReaders()
    {
        ReflectiveTypeReaderBuilder<IControl>("DynamicMenu.Controls.IControl")
            .RegisterAbstract();

        auto control = DescribeControl();
        control.RegisterAbstract<IControl>();

        auto textControl = DescribeTextControl(control);
        textControl.RegisterAbstract<IControl>();

        ContentTypeReaderManager::AddTypeCreator(
            "Microsoft.Xna.Framework.Content.ListReader`1[[DynamicMenu.Controls.IControl]]",
            [] {
                return std::unique_ptr<ContentTypeReaderBase>(
                    std::make_unique<CNA::Internal::Xnb::ListReader<std::shared_ptr<IControl>>>(
                        "System.Collections.Generic.List`1[[DynamicMenu.Controls.IControl]]",
                        ReflectiveTypeReader<IControl>::CanonicalReaderName(
                            "DynamicMenu.Controls.IControl")));
            });

        ReflectiveTypeReaderBuilder<Container>("DynamicMenu.Controls.Container")
            .Base(control)
            .Custom([](Container& value, ContentReader& input) {
                value.controls_ = input.ReadObject<std::vector<std::shared_ptr<IControl>>>();
            })
            .RegisterShared<IControl>();

        ReflectiveTypeReaderBuilder<Image>("DynamicMenu.Controls.Image")
            .Base(control)
            .RegisterShared<IControl>();

        ReflectiveTypeReaderBuilder<MultilineTextControl>(
            "DynamicMenu.Controls.MultilineTextControl")
            .Base(textControl)
            .RegisterShared<IControl>();

        ReflectiveTypeReaderBuilder<Label>("DynamicMenu.Controls.Label")
            .Base(textControl)
            .RegisterShared<IControl>();

        ReflectiveTypeReaderBuilder<Button>("DynamicMenu.Controls.Button")
            .Base(textControl)
            .Custom([](Button& value, ContentReader& input) {
                value.setPressedTextureNameProperty(input.ReadObject<std::string>());
            })
            .RegisterShared<IControl>();

        ReflectiveTypeReaderBuilder<ProgressBar>("DynamicMenu.Controls.ProgressBar")
            .Base(control)
            .Custom([](ProgressBar& value, ContentReader& input) {
                value.setLeftTextureNameProperty(input.ReadObject<std::string>());
            })
            .Custom([](ProgressBar& value, ContentReader& input) {
                value.setRightTextureNameProperty(input.ReadObject<std::string>());
            })
            .Custom([](ProgressBar& value, ContentReader& input) {
                value.setPositionProperty(input.ReadInt32());
            })
            .Custom([](ProgressBar& value, ContentReader& input) {
                value.setMaxValueProperty(input.ReadInt32());
            })
            .Custom([](ProgressBar& value, ContentReader& input) {
                value.setLeftColorProperty(input.ReadColor());
            })
            .Custom([](ProgressBar& value, ContentReader& input) {
                value.setRightColorProperty(input.ReadColor());
            })
            .Custom([](ProgressBar& value, ContentReader& input) {
                value.setBorderWidthProperty(input.ReadInt32());
            })
            .RegisterShared<IControl>();
    }
}
