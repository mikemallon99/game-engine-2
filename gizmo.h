#ifndef GIZMO_H
#define GIZMO_H

#include "geom_primitives.h"

float BOX_RADIUS = 0.1;

// Once you get this, then youll want to transform by the objects model matrix
AABB getXAxisBBox() {
    // BBox goes from (0.0, 0.0, 0.0) to (1.5, 0.0, 0.0)
    return AABB(
        0.0f, 1.5f,
        -BOX_RADIUS, BOX_RADIUS,
        -BOX_RADIUS, BOX_RADIUS
    );
}

AABB getYAxisBBox() {
    // BBox goes from (0.0, 0.0, 0.0) to (1.5, 0.0, 0.0)
    return AABB(
        -BOX_RADIUS, BOX_RADIUS,
        0.0f, 1.5f,
        -BOX_RADIUS, BOX_RADIUS
    );
}

AABB getZAxisBBox() {
    // BBox goes from (0.0, 0.0, 0.0) to (1.5, 0.0, 0.0)
    return AABB(
        -BOX_RADIUS, BOX_RADIUS,
        -BOX_RADIUS, BOX_RADIUS,
        0.0f, 1.5f
    );
}

#endif