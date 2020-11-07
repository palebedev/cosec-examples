#include "blockmetrics.hpp"

#include <QtGui/QGuiApplication>

BlockMetrics& BlockMetrics::get()
{
    static BlockMetrics bm;
    return bm;
}

BlockMetrics::BlockMetrics()
    : fontMetrics_{QGuiApplication::font()},
      portSize_{fontMetrics_.averageCharWidth()*4},
      hMargin_{fontMetrics_.averageCharWidth()},
      vMargin_{fontMetrics_.height()/2.},
      curveLead_{hMargin_*5},
      normalBrush_{QColor{255,230,130}},
      selectedBrush_{QColor{255,155,50}},
      inputPortBrush_{QColor{128,255,128}},
      outputPortBrush_{QColor{128,144,255}}
{
    selectedPen_.setWidth(2);
    double halfPen = normalPen_.widthF()/2.;
    portRect_ = {-halfPen,-halfPen,portSize_+halfPen,portSize_+halfPen};
}
