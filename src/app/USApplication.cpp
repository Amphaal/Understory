// UnderStory
// An intuitive Pen & Paper experience
// Copyright (C) 2020 Guillaume Vara

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Any graphical resources available within the source code may
// use a different license and copyright : please refer to their metadata
// for further details. Graphical resources without explicit references to a
// different license and copyright still refer to this GPL.

#include "USApplication.h"

using namespace Magnum::Math::Literals;

UnderStory::USApplication::USApplication(const Arguments& arguments): Magnum::Platform::Application{
        arguments,
        Configuration{}.setTitle(APP_FULL_DENOM)
                       .setSize(Magnum::Vector2i{MINIMUM_WIDTH, MINIMUM_HEIGHT})
                       .setWindowFlags(Configuration::WindowFlag::Resizable),
        GLConfiguration{}
    },
    _worldCache{Magnum::Vector2i{2048}, Magnum::Vector2i{512}, 22},
    _rs("data"),
    _mmh(&_timeline, &_transformationWorld),
    _msh(&_timeline, &_transformationWorld),
    _kmh(&_timeline, &_transformationWorld),
    _atomSelector(&_timeline, &_flatShader, this),
    _panel(&_timeline, &_flatShader, this),
    _selectionRect(_rs),
    _grid(_rs, MAP_SIZE, MINIMUM_HEIGHT),
    _enlighter(&_flatShader) {
    // set minimum size
    SDL_SetWindowMinimumSize(this->window(), MINIMUM_WIDTH, MINIMUM_HEIGHT);

    //
    _updateChecker.start();

    // define animation mutual exclusions
    _kmh.setExcludedWhenPlaying({&_mmh});
    _mmh.setExcludedWhenPlaying({&_kmh});

    // activate VSync
    setSwapInterval(-1);

    /* Load a TrueTypeFont plugin and open the font */
    _defaultFont = _fontManager.loadAndInstantiate("TrueTypeFont");
    if(!_defaultFont || !_defaultFont->openData(_rs.getRaw("SourceSansPro-Regular.ttf"), 180.f))
        Magnum::Fatal{} << "Cannot open font file";

    /* Glyphs we need to render everything */
    _defaultFont->fillGlyphCache(
        _worldCache,
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789:-+,.!°()/[] "
    );

    // define world text as static
    {
        Widget::StaticTextFactory textFactory(*_defaultFont, _worldCache, .2f);
        _worldText = textFactory.generate("Welcome to the Understory developper early demo !\n"
            "This is a placeholder meant to showcase the text displaying capabilities.\n"
            "Thank you for trying this early stage build and supporting our efforts !\n"
            "More infos available on our Discord server and Github page (https://github.com/Amphaal/Understory)"
        );
    }

    // define shortcut text as static
    {
        Widget::StaticTextFactory textFactory(*_defaultFont, _worldCache, 14.f);
        auto text = textFactory.generate(
                "[LeftClick  (DoubleClick)] Center screen on cursor\n"
                "[LeftClick  (Maintain)]                       Move camera\n"
                "[RightClick (Maintain / Release)]             Snapshot zoom-in\n"
                "[D-Pad]                       Move camera\n"
                "[Esc]                       Reset camera\n"
                "[MouseScroll]                                       Zoom\n"
                "[NumPad +/-]                                       Zoom",
            Magnum::Text::Alignment::TopRight
        );
        _stWidget.reset(new Widget::ShortcutsText(std::move(text), &_timeline, &_textShader, this));
    }

    /* Set up premultiplied alpha blending to avoid overlapping text characters to cut into each other */
    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::Blending);
    Magnum::GL::Renderer::setBlendFunction(Magnum::GL::Renderer::BlendFunction::SourceAlpha, Magnum::GL::Renderer::BlendFunction::OneMinusSourceAlpha);
    Magnum::GL::Renderer::setBlendEquation(Magnum::GL::Renderer::BlendEquation::Add, Magnum::GL::Renderer::BlendEquation::Add);

    // define debug text as dynamic
    _debugText.reset(new Magnum::Text::Renderer2D(*_defaultFont, _worldCache, 14.f, Magnum::Text::Alignment::TopLeft));
    _debugText->reserve(100, Magnum::GL::BufferUsage::DynamicDraw, Magnum::GL::BufferUsage::StaticDraw);
    _updateDebugText();

    //
    _timeline.start();
}

void UnderStory::USApplication::_onViewportChange() {
    //
    Widget::Constraints wh {windowSize()};

    //
    _projectionWorld = Magnum::Matrix3::projection(
        Magnum::Vector2::xScale(wh.ws.aspectRatio())
    );

    // stick to top left corner + 5 pixels padding
    _transformationProjectionDebugText = wh.baseProjMatrix *
        Magnum::Matrix3::translation(
            wh.ws *
            (Magnum::Vector2 {-.5f, .5f} - wh.pixelSize * 5 * Magnum::Vector2{-1.f, 1.f})
        );

    //
    _atomSelector.onViewportChange(wh);
    _stWidget->onViewportChange(wh);
    _panel.onViewportChange(wh);
    _selectionRect.onViewportChange();
}

void UnderStory::USApplication::viewportEvent(ViewportEvent& event) {
    Magnum::GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
    _onViewportChange();
}

void UnderStory::USApplication::drawEvent() {
    Magnum::GL::defaultFramebuffer.clear(Magnum::GL::FramebufferClear::Color);

    // advance animations
    {
        _mmh.advance();
        _msh.advance();
        _kmh.advance();
        _stWidget->advance();
        _atomSelector.advance();
        _panel.advance();
    }

    // world
    auto worldMatrix = _projectionWorld * _transformationWorld;
    auto scale = _transformationWorld.uniformScaling();
    _textShader.bindVectorTexture(_worldCache.texture());
    _textShader
        .setTransformationProjectionMatrix(worldMatrix)
        .setColor(0x2f83cc_rgbf)
        .setOutlineColor(0xdcdcdc_rgbf)
        .setOutlineRange(.45f, .35f)
        .setSmoothness(.015f/ scale)
        .draw(_worldText);

    // grid
    _grid.draw(worldMatrix, scale);

    // selection rect
    _selectionRect.mayDraw(_projectionWorld);

    // panel
    _panel.mayDraw();

    // atom selector
    _atomSelector.draw();

    // debug text
    _updateDebugText();
    _textShader
        .setColor(DEBUG_TEXT_COLOR)
        .setOutlineColor(DEBUG_TEXT_COLOR)
        .setOutlineRange(.47f, .44f)
        .setSmoothness(.33f)
        .setTransformationProjectionMatrix(_transformationProjectionDebugText)
        .draw(_debugText->mesh());

    // enlighter
    _enlighter.draw(_stWidget->enlighterColor());

    // shortcuts text
    _stWidget->draw();

    // buffers swap, automatic redrawing and timeline frame check
    swapBuffers();
    redraw();
    _timeline.nextFrame();
}

void UnderStory::USApplication::mouseScrollEvent(MouseScrollEvent& event) {
    _msh.mouseScrollEvent(event, this);
}

void UnderStory::USApplication::keyReleaseEvent(KeyEvent& event) {
    _kmh.keyReleaseEvent(event);
}

void UnderStory::USApplication::keyPressEvent(KeyEvent& event) {
    _kmh.keyPressEvent(event);
    _msh.keyPressEvent(event);

    switch (event.key()) {
    case KeyEvent::Key::Esc:
            _resetWorldMatrix();
        break;
    default:
        break;
    }
}

void UnderStory::USApplication::mouseMoveEvent(MouseMoveEvent& event) {
    // check if locked context
    if(_lockContext == this) {
        if (_mouseState.dragging()) {
            //
            this->setCursor(Cursor::ResizeAll);

            //
            _mmh.mouseMoveEvent(event, this);

        } else if (_selectionRect.isSelecting()) {
            // if selection, update rect
            _selectionRect.update(event, this);
        }

    // no locked context, update hover context
    } else if(!_lockContext) {
        _updateHoverContext(event);
    }
}

void UnderStory::USApplication::_updateHoverContext(MouseMoveEvent& event) {
    //
    auto cursorPos = _cursorPosition(event);

    // check if on atomSelector
    _atomSelector.onMouseMove(cursorPos);
    if(_atomSelector.isHovered()) {
        _hoverContext = &_atomSelector;
        return;
    }

    // check if on panel
    _panel.onMouseMove(cursorPos);
    if(_panel.isHovered()) {
        _hoverContext = &_panel;
        return;
    }

    // check if on shortcuts text
    _stWidget->onMouseMove(cursorPos);
    if(_stWidget->isHovered()) {
        _hoverContext = _stWidget.get();
        return;
    }

    // define app as context by default
    _hoverContext = this;
}

void UnderStory::USApplication::mousePressEvent(MouseEvent& event) {
    //
    _lockContext = _hoverContext;

    //
    switch (event.button()) {
        case MouseEvent::Button::Left: {
            //
            auto isDoubleClick = _mouseState.leftPressed();

            //
            if(_lockContext == this) {
                _mmh.mousePressEvent();
            }

            //
            if (isDoubleClick) leftMouseDoubleClickEvent(event);
        }
        break;

        case MouseEvent::Button::Right: {
            //
            _mouseState.rightPressed();

            //
            if(_lockContext == this) {
                _selectionRect.init(event, this);
            }
        }
        break;

        default:
        break;
    }
}

void UnderStory::USApplication::leftMouseDoubleClickEvent(MouseEvent& event) {
    if(_lockContext == this) {
        _mmh.mouseDoubleClickEvent(event, this);
    }
}

void UnderStory::USApplication::mouseReleaseEvent(MouseEvent& event) {
    //
    switch (event.button()) {
        case MouseEvent::Button::Left: {
            //
            auto dragged = _mouseState.leftReleased();

            //
            if(_lockContext == this) {
                if (dragged) {
                    _mmh.mouseMoveReleaseEvent();
                    this->setCursor(Cursor::Arrow);
                }

            } else if (_lockContext == &_atomSelector) {
                _atomSelector.toggle();
                _panel.toggle();
            }
        }
        break;

        case MouseEvent::Button::Right: {
            if(_lockContext == this) {
                if (_selectionRect.isSelecting()) {
                    _selectionRect.end(event, this);
                    _msh.snapshotZoom(_selectionRect.asRectangle());
                }
            }

            //
            _mouseState.rightReleased();
        }
        break;

        default:
        break;
    }

    //
    _lockContext = nullptr;
}

void UnderStory::USApplication::_resetWorldMatrix() {
    _mmh.stopAnim();
    _msh.stopAnim();
    _kmh.stopAnim();
    _transformationWorld = Magnum::Matrix3();
}

void UnderStory::USApplication::_updateDebugText() {
    auto us = _transformationWorld.uniformScaling();
    auto tr = _transformationWorld.translation() / Magnum::Vector2 {us};

    auto formatedText = Magnum::Utility::formatString(
        "x: {:.2}\ny: {:.2}\nZoom: x{:.2}",
        tr[0],
        tr[1],
        us
    );

    _debugText->render(formatedText);
}

Magnum::Vector2 UnderStory::USApplication::_cursorPosition(const Sdl2Application::MouseMoveEvent& event) const {
    Magnum::Vector2 fs{this->framebufferSize()};
    return (Magnum::Vector2{event.position()} - fs / 2.f) / fs * Magnum::Vector2{2.f, -2.f};
}

MAGNUM_APPLICATION_MAIN(UnderStory::USApplication)
