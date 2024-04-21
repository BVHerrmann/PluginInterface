//
//  colors.h
//  Inspector
//
//  Created by Jendrik Bertram on 12.12.19.
//

#ifndef colors_h
#define colors_h

#include <QColor>
#include <QFont>

namespace HMIFont {
    const QFont default10 = QFont("Tahoma, Siemens Sans", 10, QFont::Normal);
    const QFont bold10 = QFont("Tahoma, Siemens Sans", 10, QFont::Bold);

    const QFont AxisTitle = bold10;
    const QFont AxisLabel = default10;
}

namespace HMIColor {
    const QColor Accent = QColor(0, 95, 135);
    const QColor Light = QColor(218, 220, 224);
    const QColor DarkGrey = QColor(39, 51, 56);
    const QColor Grey = QColor(64, 77, 83);
    const QColor LightGrey = QColor(196, 199, 204);
    const QColor ContentBoard = QColor(249, 247, 248);

    const QColor WarningLow = QColor(234, 206, 33);
    const QColor WarningHigh = QColor(231, 121, 16);
    const QColor Alarm = QColor(202, 51, 51);
    const QColor OK = QColor(133, 164, 7);

    const QColor WarningLowTranslucent = QColor(234, 206, 33, 128);
    const QColor WarningHighTranslucent = QColor(231, 121, 16, 128);
    const QColor AlarmTranslucent = QColor(202, 51, 51, 127);

    const QColor red = Alarm;
    const QColor green = OK;
    const QColor blue = Accent;
    const QColor white = ContentBoard;
    const QColor yellow = WarningLow;
    const QColor orange = WarningHigh;

    const QColor transparentRed = QColor(red.red(), red.green(), red.blue(), 128);
    const QColor transparentGreen = QColor(green.red(), green.green(), green.blue(), 128);
    const QColor transparentBlue = QColor(blue.red(), blue.green(), blue.blue(), 128);
    const QColor transparentWhite = QColor(white.red(), white.green(), white.blue(), 128);
    const QColor transparentYellow = QColor(yellow.red(), yellow.green(), yellow.blue(), 128);
    const QColor transparentOrange = QColor(orange.red(), orange.green(), orange.blue(), 128);
}

#endif /* colors_h */
