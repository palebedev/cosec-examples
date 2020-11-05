#include "opaquegraphicsitem.hpp"

QPainterPath OpaqueGraphicsItem::opaqueArea() const
{
    // QGraphicsItem::shape() returns empty path by default.
    return shape();
}
