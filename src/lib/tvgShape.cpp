/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *               http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */
#ifndef _TVG_SHAPE_CPP_
#define _TVG_SHAPE_CPP_

#include "tvgCommon.h"
#include "tvgShapeImpl.h"

/************************************************************************/
/* Internal Class Implementation                                        */
/************************************************************************/
constexpr auto PATH_KAPPA = 0.552284f;


/************************************************************************/
/* External Class Implementation                                        */
/************************************************************************/

Shape :: Shape() : pImpl(make_unique<Impl>())
{
}


Shape :: ~Shape()
{
}


unique_ptr<Shape> Shape::gen() noexcept
{
    return unique_ptr<Shape>(new Shape);
}


int Shape::reset() noexcept
{
    auto impl = pImpl.get();
    assert(impl && impl->path);

    impl->path->reset();

    impl->flag |= RenderUpdateFlag::Path;

    return 0;
}


size_t Shape::pathCommands(const PathCommand** cmds) const noexcept
{
    auto impl = pImpl.get();
    assert(impl && impl->path && cmds);

    *cmds = impl->path->cmds;

    return impl->path->cmdCnt;
}


size_t Shape::pathCoords(const Point** pts) const noexcept
{
    auto impl = pImpl.get();
    assert(impl && impl->path && pts);

    *pts = impl->path->pts;

    return impl->path->ptsCnt;
}


int Shape::appendPath(const PathCommand *cmds, size_t cmdCnt, const Point* pts, size_t ptsCnt) noexcept
{
    if (cmdCnt < 0 || ptsCnt < 0) return -1;
    assert(cmds && pts);

    auto impl = pImpl.get();
    assert(impl && impl->path);

    impl->path->grow(cmdCnt, ptsCnt);
    impl->path->append(cmds, cmdCnt, pts, ptsCnt);

    impl->flag |= RenderUpdateFlag::Path;

    return 0;
}


int Shape::moveTo(float x, float y) noexcept
{
    auto impl = pImpl.get();
    assert(impl && impl->path);

    impl->path->moveTo(x, y);

    impl->flag |= RenderUpdateFlag::Path;

    return 0;
}


int Shape::lineTo(float x, float y) noexcept
{
    auto impl = pImpl.get();
    assert(impl && impl->path);

    impl->path->lineTo(x, y);

    impl->flag |= RenderUpdateFlag::Path;

    return 0;
}


int Shape::cubicTo(float cx1, float cy1, float cx2, float cy2, float x, float y) noexcept
{
    auto impl = pImpl.get();
    assert(impl && impl->path);

    impl->path->cubicTo(cx1, cy1, cx2, cy2, x, y);

    impl->flag |= RenderUpdateFlag::Path;

    return 0;
}


int Shape::close() noexcept
{
    auto impl = pImpl.get();
    assert(impl && impl->path);

    impl->path->close();

    impl->flag |= RenderUpdateFlag::Path;

    return 0;
}


int Shape::appendCircle(float cx, float cy, float radiusW, float radiusH) noexcept
{
    auto impl = pImpl.get();
    assert(impl && impl->path);

    auto halfKappaW = radiusW * PATH_KAPPA;
    auto halfKappaH = radiusH * PATH_KAPPA;

    impl->path->grow(6, 13);
    impl->path->moveTo(cx, cy - radiusH);
    impl->path->cubicTo(cx + halfKappaW, cy - radiusH, cx + radiusW, cy - halfKappaH, cx + radiusW, cy);
    impl->path->cubicTo(cx + radiusW, cy + halfKappaH, cx + halfKappaW, cy + radiusH, cx, cy + radiusH);
    impl->path->cubicTo(cx - halfKappaW, cy + radiusH, cx - radiusW, cy + halfKappaH, cx - radiusW, cy);
    impl->path->cubicTo(cx - radiusW, cy - halfKappaH, cx - halfKappaW, cy - radiusH, cx, cy - radiusH);
    impl->path->close();

    impl->flag |= RenderUpdateFlag::Path;

    return 0;
}


int Shape::appendRect(float x, float y, float w, float h, float cornerRadius) noexcept
{
    auto impl = pImpl.get();
    assert(impl && impl->path);

    //clamping cornerRadius by minimum size
    auto min = (w < h ? w : h) * 0.5f;
    if (cornerRadius > min) cornerRadius = min;

    //rectangle
    if (cornerRadius == 0) {
        impl->path->grow(5, 4);
        impl->path->moveTo(x, y);
        impl->path->lineTo(x + w, y);
        impl->path->lineTo(x + w, y + h);
        impl->path->lineTo(x, y + h);
        impl->path->close();
    //circle
    } else if (w == h && cornerRadius * 2 == w) {
        return appendCircle(x + (w * 0.5f), y + (h * 0.5f), cornerRadius, cornerRadius);
    } else {
        auto halfKappa = cornerRadius * 0.5;
        impl->path->grow(10, 17);
        impl->path->moveTo(x + cornerRadius, y);
        impl->path->lineTo(x + w - cornerRadius, y);
        impl->path->cubicTo(x + w - cornerRadius + halfKappa, y, x + w, y + cornerRadius - halfKappa, x + w, y + cornerRadius);
        impl->path->lineTo(x + w, y + h - cornerRadius);
        impl->path->cubicTo(x + w, y + h - cornerRadius + halfKappa, x + w - cornerRadius + halfKappa, y + h, x + w - cornerRadius, y + h);
        impl->path->lineTo(x + cornerRadius, y + h);
        impl->path->cubicTo(x + cornerRadius - halfKappa, y + h, x, y + h - cornerRadius + halfKappa, x, y + h - cornerRadius);
        impl->path->lineTo(x, y + cornerRadius);
        impl->path->cubicTo(x, y + cornerRadius - halfKappa, x + cornerRadius - halfKappa, y, x + cornerRadius, y);
        impl->path->close();
    }

    impl->flag |= RenderUpdateFlag::Path;

    return 0;
}


int Shape::fill(size_t r, size_t g, size_t b, size_t a) noexcept
{
    auto impl = pImpl.get();
    assert(impl);

    impl->color[0] = r;
    impl->color[1] = g;
    impl->color[2] = b;
    impl->color[3] = a;
    impl->flag |= RenderUpdateFlag::Fill;

    return 0;
}


int Shape::fill(size_t* r, size_t* g, size_t* b, size_t* a) const noexcept
{
    auto impl = pImpl.get();
    assert(impl);

    if (r) *r = impl->color[0];
    if (g) *g = impl->color[1];
    if (b) *b = impl->color[2];
    if (a) *a = impl->color[3];

    return 0;
}


int Shape::scale(float factor) noexcept
{
    auto impl = pImpl.get();
    assert(impl);

    if (fabsf(factor) < FLT_EPSILON || fabsf(factor - impl->scale) <= FLT_EPSILON) return -1;

    impl->scale = factor;
    impl->flag |= RenderUpdateFlag::Transform;

    return 0;
}


int Shape::rotate(float degree) noexcept
{
    auto impl = pImpl.get();
    assert(impl);

    if (fabsf(degree - impl->rotate) <= FLT_EPSILON) return -1;

    impl->rotate = degree;
    impl->flag |= RenderUpdateFlag::Transform;

    return 0;
}


int Shape::bounds(float& x, float& y, float& w, float& h) const noexcept
{
    auto impl = pImpl.get();
    assert(impl && impl->path);

    if (!impl->path->bounds(x, y, w, h)) return -1;

    return 0;
}


float Shape::scale() const noexcept
{
    auto impl = pImpl.get();
    assert(impl);

    return impl->scale;
}


float Shape::rotate() const noexcept
{
    auto impl = pImpl.get();
    assert(impl);

    return impl->rotate;
}

#endif //_TVG_SHAPE_CPP_
