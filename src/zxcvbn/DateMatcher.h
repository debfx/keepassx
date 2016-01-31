/*
 *  Copyright (C) 2012-2015 Dan Wheeler and Dropbox, Inc.
 *  Copyright (C) 2016 Felix Geyer <debfx@fobos.de>
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the
 *  "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so, subject to
 *  the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 *  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef ZXCVBN_DATEMATCHER_H
#define ZXCVBN_DATEMATCHER_H

#include "Matcher.h"

#include <QDate>
#include <QHash>

namespace Zxcvbn {

class DateMatcher : public Matcher
{
public:
    QList<Match> match(const QString& password) override;

private:
    static QDate mapIntsToDmy(const QList<int>& ints);
    static QPair<int,int> mapIntsToDm(int a, int b);
    static int twoToFourDigitYear(int year);
    static bool isSubmatch(const Match& match, const QList<Match>& allMatches);

    static const int DATE_MAX_YEAR;
    static const int DATE_MIN_YEAR;
    static const QHash<int, QList<QPair<int,int>>> DATE_SPLITS;
};

} // namespace Zxcvbn

#endif // ZXCVBN_DATEMATCHER_H
