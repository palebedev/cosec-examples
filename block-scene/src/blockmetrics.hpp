#ifndef UUID_5E459A6E_83DA_47CC_B0C4_55E199468B34
#define UUID_5E459A6E_83DA_47CC_B0C4_55E199468B34

#include <QtGui/QBrush>
#include <QtGui/QFontMetricsF>
#include <QtGui/QPen>

// For simplicity, we will not account for
// QGuiApplication::fontChanged and other events.
class BlockMetrics
{
public:
    static BlockMetrics& get();

    const QFontMetricsF& fontMetrics() const noexcept
    {
        return fontMetrics_;
    }

    double portSize() const noexcept
    {
        return portSize_;
    }

    double hMargin() const noexcept
    {
        return hMargin_;
    }        

    double vMargin() const noexcept
    {
        return vMargin_;
    }

    double curveLead() const noexcept
    {
        return curveLead_;
    }

    QRectF portRect() const noexcept
    {
        return portRect_;
    }

    const QPen& normalPen() const noexcept
    {
        return normalPen_;
    }        

    const QPen& selectedPen() const noexcept
    {
        return selectedPen_;
    }

    const QBrush& normalBrush() const noexcept
    {
        return normalBrush_;
    }

    const QBrush& selectedBrush() const noexcept
    {
        return selectedBrush_;
    }

    const QBrush& inputPortBrush() const noexcept
    {
        return inputPortBrush_;
    }

    const QBrush& outputPortBrush() const noexcept
    {
        return outputPortBrush_;
    }

    double portTop(int i) const noexcept
    {
        return i*portSize_+(i+1)*vMargin_;
    }
private:
    QFontMetricsF fontMetrics_;
    double portSize_,hMargin_,vMargin_,curveLead_;
    QRectF portRect_;
    QPen normalPen_,selectedPen_;
    QBrush normalBrush_,selectedBrush_,inputPortBrush_,outputPortBrush_;

    BlockMetrics();
};

#endif
