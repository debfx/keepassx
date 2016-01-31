/*
 *  Copyright (C) 2012-2015 Dan Wheeler and Dropbox, Inc.
 *  Copyright (C) 2015 Felix Geyer <debfx@fobos.de>
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

#include "TestZxcvbn.h"

#include <QTest>

#include "zxcvbn/DateMatcher.h"
#include "zxcvbn/RegexMatcher.h"
#include "zxcvbn/RepeatMatcher.h"
#include "zxcvbn/SequenceMatcher.h"

QTEST_GUILESS_MAIN(TestZxcvbn)

void TestZxcvbn::testRepeatMatching()
{
    Zxcvbn::RepeatMatcher matcher;

    for (const QString& password : {"", "#"}) {
        const QList<Zxcvbn::Match> matches = matcher.match(password);

        QCOMPARE(matches.size(), 0);
    }

    // test single-character repeats
    QStringList prefixes = { "@", "y4@" };
    QStringList suffixes = { "u", "u%7" };
    QString pattern = "&&&&&";

    Q_FOREACH (const GeneratedPassword& genPw, genPws(pattern, prefixes, suffixes)) {
        const QList<Zxcvbn::Match> matches = matcher.match(genPw.password);

        QCOMPARE(matches.size(), 1);
        QCOMPARE(matches.at(0).value("i").toInt(), genPw.i);
        QCOMPARE(matches.at(0).value("j").toInt(), genPw.j);
        QCOMPARE(matches.at(0).value("token").toString(), pattern);
        QCOMPARE(matches.at(0).value("baseToken").toString(), QString("&"));
    }

    for (int length : {3, 12}) {
        for (QChar chr : {'a', 'Z', '4', '&'}) {
            QString pattern = QString(length + 1, chr);
            const QList<Zxcvbn::Match> matches = matcher.match(pattern);

            QCOMPARE(matches.size(), 1);
            QCOMPARE(matches.at(0).value("i").toInt(), 0);
            QCOMPARE(matches.at(0).value("j").toInt(), pattern.size() - 1);
            QCOMPARE(matches.at(0).value("token").toString(), pattern);
            QCOMPARE(matches.at(0).value("baseToken").toString(), QString(chr));
        }
    }

    {
        const QList<Zxcvbn::Match> matches = matcher.match("BBB1111aaaaa@@@@@@");
        QCOMPARE(matches.size(), 4);

        QCOMPARE(matches.at(0).value("i").toInt(), 0);
        QCOMPARE(matches.at(0).value("j").toInt(), 2);
        QCOMPARE(matches.at(0).value("token").toString(), QString("BBB"));
        QCOMPARE(matches.at(0).value("baseToken").toString(), QString("B"));

        QCOMPARE(matches.at(1).value("i").toInt(), 3);
        QCOMPARE(matches.at(1).value("j").toInt(), 6);
        QCOMPARE(matches.at(1).value("token").toString(), QString("1111"));
        QCOMPARE(matches.at(1).value("baseToken").toString(), QString("1"));

        QCOMPARE(matches.at(2).value("i").toInt(), 7);
        QCOMPARE(matches.at(2).value("j").toInt(), 11);
        QCOMPARE(matches.at(2).value("token").toString(), QString("aaaaa"));
        QCOMPARE(matches.at(2).value("baseToken").toString(), QString("a"));

        QCOMPARE(matches.at(3).value("i").toInt(), 12);
        QCOMPARE(matches.at(3).value("j").toInt(), 17);
        QCOMPARE(matches.at(3).value("token").toString(), QString("@@@@@@"));
        QCOMPARE(matches.at(3).value("baseToken").toString(), QString("@"));
    }

    {
        const QList<Zxcvbn::Match> matches = matcher.match("2818BBBbzsdf1111@*&@!aaaaaEUDA@@@@@@1729");
        QCOMPARE(matches.size(), 4);

        QCOMPARE(matches.at(0).value("i").toInt(), 4);
        QCOMPARE(matches.at(0).value("j").toInt(), 6);
        QCOMPARE(matches.at(0).value("token").toString(), QString("BBB"));
        QCOMPARE(matches.at(0).value("baseToken").toString(), QString("B"));

        QCOMPARE(matches.at(1).value("i").toInt(), 12);
        QCOMPARE(matches.at(1).value("j").toInt(), 15);
        QCOMPARE(matches.at(1).value("token").toString(), QString("1111"));
        QCOMPARE(matches.at(1).value("baseToken").toString(), QString("1"));

        QCOMPARE(matches.at(2).value("i").toInt(), 21);
        QCOMPARE(matches.at(2).value("j").toInt(), 25);
        QCOMPARE(matches.at(2).value("token").toString(), QString("aaaaa"));
        QCOMPARE(matches.at(2).value("baseToken").toString(), QString("a"));

        QCOMPARE(matches.at(3).value("i").toInt(), 30);
        QCOMPARE(matches.at(3).value("j").toInt(), 35);
        QCOMPARE(matches.at(3).value("token").toString(), QString("@@@@@@"));
        QCOMPARE(matches.at(3).value("baseToken").toString(), QString("@"));
    }

    {
        const QList<Zxcvbn::Match> matches = matcher.match("abab");
        QCOMPARE(matches.size(), 1);

        QCOMPARE(matches.at(0).value("i").toInt(), 0);
        QCOMPARE(matches.at(0).value("j").toInt(), 3);
        QCOMPARE(matches.at(0).value("token").toString(), QString("abab"));
        QCOMPARE(matches.at(0).value("baseToken").toString(), QString("ab"));
    }

    {
        const QList<Zxcvbn::Match> matches = matcher.match("aabaab");
        QCOMPARE(matches.size(), 1);

        QCOMPARE(matches.at(0).value("i").toInt(), 0);
        QCOMPARE(matches.at(0).value("j").toInt(), 5);
        QCOMPARE(matches.at(0).value("token").toString(), QString("aabaab"));
        QCOMPARE(matches.at(0).value("baseToken").toString(), QString("aab"));
    }

    {
        const QList<Zxcvbn::Match> matches = matcher.match("abababab");
        QCOMPARE(matches.size(), 1);

        QCOMPARE(matches.at(0).value("i").toInt(), 0);
        QCOMPARE(matches.at(0).value("j").toInt(), 7);
        QCOMPARE(matches.at(0).value("token").toString(), QString("abababab"));
        QCOMPARE(matches.at(0).value("baseToken").toString(), QString("ab"));
    }
}

void printMatch(const Zxcvbn::Match& match)
{
    for (const QString& key : match.keys()) {
        qWarning("%s: %s", key.toLocal8Bit().constData(), match.value(key).toString().toLocal8Bit().constData());
    }
}

void TestZxcvbn::testSequenceMatching()
{
    QFETCH(QString, password);
    QFETCH(QVariantList, expectedList);

    Zxcvbn::SequenceMatcher matcher;
    const QList<Zxcvbn::Match> matches = matcher.match(password);

    // redundant, but gives us a better error message
    if (matches.size() != expectedList.size()) {
        for (int i = 0; i < matches.size(); i++) {
            if (i != 0) {
                qWarning("---");
            }
            printMatch(matches[i]);
        }
    }

    QCOMPARE(matches.size(), expectedList.size());

    for (int i = 0; i < matches.size(); i++) {
        QVariantMap expected = expectedList[i].toMap();

        for (const QVariant& keyVariant : expected.keys()) {
            const QString key = keyVariant.toString();

            // redundant, but gives us a better error message
            if (!matches[i].contains(key) || (matches[i].value(key) != expected.value(key))) {
                qWarning("Testing match %d, key '%s':", i, key.toLocal8Bit().constData());
                printMatch(matches[i]);
            }

            QVERIFY(matches[i].contains(key));
            QCOMPARE(matches[i].value(key), expected.value(key));
        }
    }
}

void TestZxcvbn::testSequenceMatching_data()
{
    QTest::addColumn<QString>("password");
    QTest::addColumn<QVariantList>("expectedList");

    QTest::newRow("") << "" << QVariantList();
    QTest::newRow("a") << "a" << QVariantList();
    QTest::newRow("1") << "1" << QVariantList();

    addRow("abcbabc", QVariantList() << QVariantMap({ {"token", "abc"}, {"i", 0}, {"j", 2}, {"ascending", true} })
                                     << QVariantMap({ {"token", "cba"}, {"i", 2}, {"j", 4}, {"ascending", false} })
                                     << QVariantMap({ {"token", "abc"}, {"i", 4}, {"j", 6}, {"ascending", true} }));

    QStringList prefixes = { "!", "22" };
    QStringList suffixes = { "!", "22" };
    QString pattern = "jihg";

    Q_FOREACH (const GeneratedPassword& genPw, genPws(pattern, prefixes, suffixes)) {
        addRow(genPw.password,
               QVariantList() << QVariantMap({ {"token", pattern}, {"i", genPw.i}, {"j", genPw.j}, {"ascending", false}, {"sequenceName", "lower"} }));
    }

    QList<QVariantList> testData = {
        {"ABC",   "upper",  true},
        {"CBA",   "upper",  false},
        {"PQR",   "upper",  true},
        {"RQP",   "upper",  false},
        {"XYZ",   "upper",  true},
        {"ZYX",   "upper",  false},
        {"abcd",  "lower",  true},
        {"dcba",  "lower",  false},
        {"jihg",  "lower",  false},
        {"wxyz",  "lower",  true},
        {"zyxw",  "lower",  false},
        {"01234", "digits", true},
        {"43210", "digits", false},
        {"67890", "digits", true},
        {"09876", "digits", false},
    };

    Q_FOREACH (const QVariantList& expected, testData) {
        QString pattern = expected.value(0).toString();
        QString name = expected.value(1).toString();
        bool isAscending = expected.value(2).toBool();

        addRow(pattern,
               QVariantList() << QVariantMap({ {"token", pattern}, {"i", 0}, {"j", pattern.size() - 1}, {"ascending", isAscending}, {"sequenceName", name} }));
    }
}

void TestZxcvbn::testRegexMatching()
{
    QFETCH(QString, password);
    QFETCH(QVariantList, expectedList);

    Zxcvbn::RegexMatcher matcher;
    const QList<Zxcvbn::Match> matches = matcher.match(password);

    // redundant, but gives us a better error message
    if (matches.size() != expectedList.size()) {
        for (int i = 0; i < matches.size(); i++) {
            if (i != 0) {
                qWarning("---");
            }
            printMatch(matches[i]);
        }
    }

    QCOMPARE(matches.size(), expectedList.size());

    for (int i = 0; i < matches.size(); i++) {
        QVariantMap expected = expectedList[i].toMap();

        for (const QVariant& keyVariant : expected.keys()) {
            const QString key = keyVariant.toString();

            // redundant, but gives us a better error message
            if (!matches[i].contains(key) || (matches[i].value(key) != expected.value(key))) {
                qWarning("Testing match %d, key '%s':", i, key.toLocal8Bit().constData());
                printMatch(matches[i]);
            }

            QVERIFY(matches[i].contains(key));
            QCOMPARE(matches[i].value(key), expected.value(key));
        }
    }
}

void TestZxcvbn::testRegexMatching_data()
{
    QTest::addColumn<QString>("password");
    QTest::addColumn<QVariantList>("expectedList");

    addRow("1922", QVariantList() << QVariantMap({ {"token", "1922"}, {"i", 0}, {"j", 3}, {"regexName", "recent_year"} }));
    addRow("2017", QVariantList() << QVariantMap({ {"token", "2017"}, {"i", 0}, {"j", 3}, {"regexName", "recent_year"} }));
}

void TestZxcvbn::testDateMatching()
{
    QFETCH(QString, password);
    QFETCH(QVariantList, expectedList);

    Zxcvbn::DateMatcher matcher;
    const QList<Zxcvbn::Match> matches = matcher.match(password);

    // redundant, but gives us a better error message
    if (matches.size() != expectedList.size()) {
        for (int i = 0; i < matches.size(); i++) {
            if (i != 0) {
                qWarning("---");
            }
            printMatch(matches[i]);
        }
    }

    QCOMPARE(matches.size(), expectedList.size());

    for (int i = 0; i < matches.size(); i++) {
        QVariantMap expected = expectedList[i].toMap();

        for (const QVariant& keyVariant : expected.keys()) {
            const QString key = keyVariant.toString();

            // redundant, but gives us a better error message
            if (!matches[i].contains(key) || (matches[i].value(key) != expected.value(key))) {
                qWarning("Testing match %d, key '%s':", i, key.toLocal8Bit().constData());
                printMatch(matches[i]);
            }

            QVERIFY(matches[i].contains(key));
            QCOMPARE(matches[i].value(key), expected.value(key));
        }
    }
}

void TestZxcvbn::testDateMatching_data()
{
    QTest::addColumn<QString>("password");
    QTest::addColumn<QVariantList>("expectedList");

    QStringList separators = { "", " ", "-", "/", "\\", "_", "." };
    for (const QString& sep : separators) {
        QString password = QString("13#{sep}2#{sep}1921").replace("#{sep}", sep);
        addRow(password, QVariantList() << QVariantMap({ {"token", password}, {"i", 0}, {"j", password.size() - 1},
                                                         {"separator", sep},
                                                         {"year", 1921}, {"month", 2}, {"day", 13} }));
    }

    QStringList orderList = { "mdy", "dmy", "ymd", "ydm" };
    for (const QString& order : orderList) {
        QString d = "8";
        QString m = "8";
        QString y = "88";
        QString password = QString(order).replace("d", d).replace("m", m).replace("y", y);
        addRow(password, QVariantList() << QVariantMap({ {"token", password}, {"i", 0}, {"j", password.size() - 1},
                                                         {"separator", ""},
                                                         {"year", 1988}, {"month", 8}, {"day", 8} }));
    }

    // matches the date with year closest to REFERENCE_YEAR when ambiguous
    QString password = "111504";
    addRow(password, QVariantList() << QVariantMap({ {"token", password}, {"i", 0}, {"j", password.size() - 1},
                                                     {"separator", ""},
                                                     {"year", 2004}, {"month", 11}, {"day", 15} }));

    QList<QList<int>> dmy = {
        {1,  1,  1999},
        {11, 8,  2000},
        {9,  12, 2005},
        {22, 11, 1551},
    };

    for (const QList<int>& date : dmy) {
        QString password;

        password = QString("%1%2%3").arg(date[0]).arg(date[1]).arg(date[2]);
        addRow(password, QVariantList() << QVariantMap({ {"token", password}, {"i", 0}, {"j", password.size() - 1},
                                                         {"separator", ""}, {"year", date[2]} }));

        password = QString("%1.%2.%3").arg(date[0]).arg(date[1]).arg(date[2]);
        addRow(password, QVariantList() << QVariantMap({ {"token", password}, {"i", 0}, {"j", password.size() - 1},
                                                         {"separator", "."}, {"year", date[2]} }));
    }

    password = "02/02/02";
    addRow(password, QVariantList() << QVariantMap({ {"token", password}, {"i", 0}, {"j", password.size() - 1},
                                                     {"separator", "/"},
                                                     {"year", 2002}, {"month", 2}, {"day", 2} }));

    QStringList prefixes = { "a", "ab" };
    QStringList suffixes = { "!" };
    QString pattern = "1/1/91";

    Q_FOREACH (const GeneratedPassword& genPw, genPws(pattern, prefixes, suffixes)) {
        addRow(genPw.password, QVariantList() << QVariantMap({ {"token", pattern}, {"i", genPw.i}, {"j", genPw.j},
                                                               {"year", 1991}, {"month", 1}, {"day", 1} }));
    }

    password = "12/20/1991.12.20";
    addRow(password, QVariantList() << QVariantMap({ {"token", "12/20/1991"}, {"i", 0}, {"j", 9},
                                                     {"separator", "/"},
                                                     {"year", 1991}, {"month", 12}, {"day", 20} })
                                    << QVariantMap({ {"token", "1991.12.20"}, {"i", 6}, {"j", 15},
                                                     {"separator", "."},
                                                     {"year", 1991}, {"month", 12}, {"day", 20} }));

    password = "912/20/919";
    addRow(password, QVariantList() << QVariantMap({ {"token", "12/20/91"}, {"i", 1}, {"j", 8},
                                                     {"separator", "/"},
                                                     {"year", 1991}, {"month", 12}, {"day", 20} }));
}

void TestZxcvbn::addRow(const QString& name, const QVariantList& expectedList)
{
    QTest::newRow(name.toLocal8Bit().constData()) << name << expectedList;
}

QList<GeneratedPassword> TestZxcvbn::genPws(const QString& pattern, QStringList prefixes, QStringList suffixes)
{
    if (!prefixes.contains("")) {
        prefixes.append("");
    }

    if (!suffixes.contains("")) {
        suffixes.append("");
    }

    QList<GeneratedPassword> result;

    for (const QString& prefix : prefixes) {
        for (const QString& suffix : suffixes) {
            GeneratedPassword generated;

            generated.password = prefix + pattern + suffix;
            generated.i = prefix.size();
            generated.j = prefix.size() + pattern.size() - 1;

            result.append(generated);
        }
    }

    return result;
}
