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

#include "ScrollablePanel.h"

#include <stdexcept>

using namespace Magnum::Math::Literals;

UnderStory::Widget::ScrollablePanel::ScrollablePanel(StickTo stickness, float thickness) :
    PlayerMatrixAnimator(&_matrix, .1f, &_defaultAnimationCallback),
    _stickness(stickness),
    _thickness(thickness),
    _scroller(this) {
    // set collapsed state as default
    _definePanelPosition(_collapsedTransform());

    //
    _initContaining({&_scroller});
}

void UnderStory::Widget::ScrollablePanel::_bindContent(Scissorable* content) {
    //
    _content = content;

    //
    auto asHoverable = dynamic_cast<Hoverable*>(content);
    if(!asHoverable) throw std::logic_error("Trying to bind a Scissorable which is not Hoverable !");

    //
    _pushContaining(asHoverable);
}

void UnderStory::Widget::ScrollablePanel::mayDraw() {
    //
    if(!isToggled() && !isAnimationPlaying()) return;

    // draw background
    Shaders::flat
        ->setTransformationProjectionMatrix(_matrix)
        .setColor(0x000000AA_rgbaf)
        .draw(_mesh);

    // draw scroller
    _scroller.mayDraw();

    // draw content
    _content->draw();
}

const Magnum::Matrix3& UnderStory::Widget::ScrollablePanel::matrix() const {
    return _matrix;
}

void UnderStory::Widget::ScrollablePanel::onMouseScroll(const Magnum::Vector2& scrollOffset) {
    _content->onMouseScroll(scrollOffset);
    _scroller.onMouseScroll(_content->scrollMatrix());
}

const UnderStory::Widget::StickTo UnderStory::Widget::ScrollablePanel::stickyness() const {
    return _stickness;
}

void UnderStory::Widget::ScrollablePanel::_defaultAnimationCallback(Magnum::Float /*t*/, const float &prc, Animation::State<Magnum::Vector2>& state) {
    //
    state.current = Magnum::Math::lerp(
        state.from,
        state.to,
        Magnum::Animation::Easing::cubicOut(prc)
    );
}

void UnderStory::Widget::ScrollablePanel::_onHoverChanged(bool isHovered) {
    if(isHovered) {
        _scroller.reveal();
    } else {
        _scroller.fade();
    }
}

void UnderStory::Widget::ScrollablePanel::_onAnimationProgress() {
    _definePanelPosition(currentAnim());
    _updateGeometry();
}

void UnderStory::Widget::ScrollablePanel::_onToggled(bool isToggled) {
    if(isToggled)
        _updateAnimationAndPlay(_collapsedTransform(), {});
    else
        _updateAnimationAndPlay({}, _collapsedTransform());
}

// helper
void UnderStory::Widget::ScrollablePanel::_updateGeometry() {
    Hoverable::_updateGeometry(_matrix);
    _scroller.updateGeometry();
}

void UnderStory::Widget::ScrollablePanel::onViewportChange(Magnum::Range2D& shapeAllowedSpace) {
    Container::onViewportChange(shapeAllowedSpace);
    _updateGeometry();
}

void UnderStory::Widget::ScrollablePanel::_definePanelPosition(const Magnum::Vector2 &pos) {
    _updateAnimatedMatrix(
        Magnum::Matrix3::translation(pos)
    );
}

UnderStory::Widget::Scroller& UnderStory::Widget::ScrollablePanel::scroller() {
    return _scroller;
}

const Magnum::Vector2 UnderStory::Widget::ScrollablePanel::_collapsedTransform() const {
    switch (_stickness) {
        case StickTo::Left :
            return {-_thickness, .0f};
        case StickTo::Top :
            return {.0f, _thickness};
        case StickTo::Right :
            return {_thickness, .0f};
        case StickTo::Bottom :
            return {.0f, -_thickness};
    }
    return {};
}

void UnderStory::Widget::ScrollablePanel::_availableSpaceChanged(Magnum::Range2D& availableSpace) {
    //
    if(_setupDone) return;

    // as shape is immuable, set it once
    {
        Magnum::Vector2 pStart, pEnd;
        auto &masterShape = availableSpace;

        // determine shape and position
        switch (_stickness) {
            case StickTo::Left :
                pStart = masterShape.min();
                pEnd = {pStart.x() + _thickness, masterShape.max().y()};
                break;
            case StickTo::Top :
                pStart = {masterShape.min().x(), masterShape.max().y() - _thickness};
                pEnd = masterShape.max();
                break;
            case StickTo::Right :
                pStart = {masterShape.max().x() - _thickness, masterShape.min().y()};
                pEnd = masterShape.max();
                break;
            case StickTo::Bottom :
                pStart = masterShape.min();
                pEnd = {masterShape.max().x(), pStart.y() + _thickness};
                break;
        }

        // update shape
        this->_updateShape({pStart, pEnd});
    }

    // define vertices
    struct Vertex {
        Magnum::Vector2 position;
    };
    const Vertex vertices[] {
        {shape().bottomLeft()},
        {shape().bottomRight()},
        {shape().topRight()},
        {shape().topLeft()}
    };

    // define indices
    Magnum::GL::Buffer bIndices, bVertices;
    bIndices.setData({
        0, 1, 2,
        2, 3, 0
    });

    // bind buffer
    bVertices.setData(vertices, Magnum::GL::BufferUsage::StaticDraw);

    // define panel mesh
    _mesh.setCount(bIndices.size())
            .setIndexBuffer (std::move(bIndices),  0, Magnum::MeshIndexType::UnsignedInt)
            .addVertexBuffer(std::move(bVertices), 0, Magnum::Shaders::Flat2D::Position{});

    //
    _setupDone = true;
}
