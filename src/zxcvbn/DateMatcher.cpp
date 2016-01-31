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

#include "DateMatcher.h"

#include <QRegularExpression>

const int Zxcvbn::DateMatcher::DATE_MAX_YEAR(2050);
const int Zxcvbn::DateMatcher::DATE_MIN_YEAR(1000);
const QHash<int, QList<QPair<int,int>>> Zxcvbn::DateMatcher::DATE_SPLITS = {
    { 4, {
          {1, 2},
          {2, 3},
         }
    },

    { 5, {
          {1, 3},
          {2, 3},
         }
    },

    { 6, {
          {1, 2},
          {2, 4},
          {4, 5},
         }
    },

    { 7, {
          {1, 3},
          {2, 3},
          {4, 5},
          {4, 6},
         }
    },

    { 8, {
          {2, 4},
          {4, 6},
         }
    },
};

QList<Zxcvbn::Match> Zxcvbn::DateMatcher::match(const QString& password)
{
    QList<Match> matches;

    QRegularExpression maybeDateNoSeparator("^\\d{4,8}$");
    QRegularExpression maybeDateWithSeparator(
        "^"
        "(\\d{1,4})"
        "([\\s/\\\\_.-])"
        "(\\d{1,2})"
        "\\2"
        "(\\d{1,4})"
        "$"
    );

    for (int i = 0; i <= password.size() - 4; i++) {
        for (int j = i + 3; j <= (i + 7); j++) {
            if (j >= password.size()) {
                break;
            }

            QString token = password.mid(i, j - i + 1);
            if (!maybeDateNoSeparator.match(token).hasMatch()) {
                continue;
            }

            QList<QDate> candidates;
            for (const QPair<int,int> split : DATE_SPLITS[token.size()]) {
                QList<int> ints;
                ints.append(token.left(split.first).toInt());
                ints.append(token.mid(split.first, split.second - split.first).toInt());
                ints.append(token.mid(split.second).toInt());

                QDate dmy = mapIntsToDmy(ints);
                if (!dmy.isNull()) {
                    candidates.append(dmy);
                }
            }

            if (candidates.isEmpty()) {
                continue;
            }

            // at this point: different possible dmy mappings for the same i,j substring.
            // match the candidate date that likely takes the fewest guesses: a year closest to 2000.
            // (scoring.REFERENCE_YEAR).
            //
            // ie, considering '111504', prefer 11-15-04 to 1-1-1504
            // (interpreting '04' as 2004)

            QDate bestCandidate = candidates.first();
            // TODO: 2000 -> scoring.REFERENCE_YEAR
            int minDistance = qAbs(bestCandidate.year() - 2000);
            for (const QDate& candidate : candidates.mid(1)) {
                // TODO: 2000 -> scoring.REFERENCE_YEAR
                int distance = qAbs(candidate.year() - 2000);
                if (distance < minDistance) {
                    bestCandidate = candidate;
                    minDistance = distance;
                }
            }
            matches.append({
                { "pattern", "date" },
                { "i", i },
                { "j", j },
                { "token", token },
                { "separator", "" },
                { "year", bestCandidate.year() },
                { "month", bestCandidate.month() },
                { "day", bestCandidate.day() },
            });
        }
    }

    // dates with separators are between length 6 '1/1/91' and 10 '11/11/1991'
    for (int i = 0; i <= (password.size() - 6); i++) {
        for (int j = i + 5; j <= (i + 9); j++) {
            if (j >= password.size()) {
                break;
            }

            qWarning("i=%d, j=%d", i, j);

            QString token = password.mid(i, j - i + 1);
            QRegularExpressionMatch rxMatch = maybeDateWithSeparator.match(token);
            if (!rxMatch.hasMatch()) {
                continue;
            }

            QList<int> ints;
            ints.append(rxMatch.captured(1).toInt());
            ints.append(rxMatch.captured(3).toInt());
            ints.append(rxMatch.captured(4).toInt());

            QDate dmy = mapIntsToDmy(ints);

            if (dmy.isNull()) {
                qWarning("skip %s: %s | %s | %s", qPrintable(token), qPrintable(rxMatch.captured(1)), qPrintable(rxMatch.captured(3)), qPrintable(rxMatch.captured(4)));
                continue;
            }

            matches.append({
                { "pattern", "date" },
                { "i", i },
                { "j", j },
                { "token", token },
                { "separator", rxMatch.captured(2) },
                { "year", dmy.year() },
                { "month", dmy.month() },
                { "day", dmy.day() },
            });
        }
    }

    // matches now contains all valid date strings in a way that is tricky to capture
    // with regexes only. while thorough, it will contain some unintuitive noise:
    //
    // '2015_06_04', in addition to matching 2015_06_04, will also contain
    // 5(!) other date matches: 15_06_04, 5_06_04, ..., even 2015 (matched as 5/1/2020)
    //
    // to reduce noise, remove date matches that are strict substrings of others

    matches.erase(std::remove_if(matches.begin(),
                                 matches.end(),
                                 [&matches](const Match& match){return isSubmatch(match, matches);}),
                  matches.end());

    std::sort(matches.begin(), matches.end(), [](const Match& a, const Match& b) {
        if (a.value("i").toInt() == b.value("i").toInt()) {
            return a.value("j").toInt() <= b.value("j").toInt();
        }
        else {
            return a.value("i").toInt() <= b.value("i").toInt();
        }
    });

    return matches;
}

QDate Zxcvbn::DateMatcher::mapIntsToDmy(const QList<int>& ints)
{
    if (ints.size() != 3 || ints[1] > 31 || ints[1] <= 0) {
        qWarning("veryveryearlyskip");
        return QDate();
    }

    int over12 = 0;
    int over31 = 0;
    int under1 = 0;

    for (int i : ints) {
        if ((99 < i && i < DATE_MIN_YEAR) || i > DATE_MAX_YEAR) {
            return QDate();
        }

        if (i > 31) {
            over31++;
        }

        if (i > 12) {
            over12++;
        }

        if (i <= 0) {
            under1++;
        }
    }

    if (over31 >= 2 || over12 == 3 || under1 >= 2) {
        qWarning("earlyskip");
        return QDate();
    }

    // first look for a four digit year: yyyy + daymonth or daymonth + yyyy
    QList<QList<int>> possibleYearSplits = {
        { ints[2], ints[0], ints[1] },
        { ints[0], ints[1], ints[2] },
    };

    for (const QList<int> split : possibleYearSplits) {
        int y = split[0];
        QList<int> rest = split.mid(1, 2);

        if (DATE_MIN_YEAR <= y && y <= DATE_MAX_YEAR) {
            QPair<int,int> dm = mapIntsToDm(rest[0], rest[1]);
            if (dm.first != -1) {
                return QDate(y, dm.second, dm.first);
            }
            else {
                // for a candidate that includes a four-digit year,
                // when the remaining ints don't match to a day and month,
                // it is not a date.
                qWarning("lateskip");
                return QDate();
            }
        }
    }

    // given no four-digit year, two digit years are the most flexible int to match, so
    // try to parse a day-month out of ints[0..1] or ints[1..0]
    for (const QList<int> split : possibleYearSplits) {
        int y = split[0];
        QList<int> rest = split.mid(1, 2);

        QPair<int,int> dm = mapIntsToDm(rest[0], rest[1]);

        if (dm.first != -1) {
            return QDate(twoToFourDigitYear(y), dm.second, dm.first);
        }
    }

    return QDate();
}

QPair<int,int> Zxcvbn::DateMatcher::mapIntsToDm(int a, int b)
{
    QList<QPair<int,int>> combinations;
    combinations.append(QPair<int,int>(a, b));
    combinations.append(QPair<int,int>(b, a));

    for (QPair<int,int> comb : combinations) {
        int d = comb.first;
        int m = comb.second;

        if ((1 <= d && d <= 31) && (1 <= m && m <= 12)) {
            return QPair<int,int>(d, m);
        }
    }

    return QPair<int,int>(-1, -1);
}

int Zxcvbn::DateMatcher::twoToFourDigitYear(int year)
{
    if (year > 99) {
        return year;
    }
    else if (year > 50) {
        // 87 -> 1987
        // TODO: 2000 -> scoring.REFERENCE_YEAR
        return year + 2000 - 100;
    }
    else {
        // 15 -> 2015
        // TODO: 2000 -> scoring.REFERENCE_YEAR
        return year + 2000;
    }
}

bool Zxcvbn::DateMatcher::isSubmatch(const Zxcvbn::Match& match, const QList<Zxcvbn::Match>& allMatches)
{
    for (const Match& otherMatch : allMatches) {
        if (otherMatch == match) {
            continue;
        }

        if ((otherMatch["i"].toInt() <= match["i"].toInt()) && (otherMatch["j"].toInt() >= match["j"].toInt())) {
            return true;
        }
    }

    return false;
}
