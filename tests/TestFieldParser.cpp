/*
 *  Copyright (C) 2014 Felix Geyer <debfx@fobos.de>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 or (at your option)
 *  version 3 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "TestFieldParser.h"

#include <QTest>

#include "tests.h"
#include "core/FieldParser.h"

void TestFieldParser::initTestCase()
{
    m_fieldParser = new FieldParser(this);
    connect(m_fieldParser, SIGNAL(field(QString,int,int)), SLOT(addField(QString,int,int)));
    connect(m_fieldParser, SIGNAL(rawString(QString)), SLOT(addRawString(QString)));
}

void TestFieldParser::cleanup()
{
    m_fields.clear();
    m_rawStrings.clear();
}

void TestFieldParser::addField(const QString& str, int pos, int len)
{
    Q_UNUSED(pos)

    QCOMPARE(str.length(), len - 2);
    m_fields.append(str);
}

void TestFieldParser::addRawString(const QString& str)
{
    m_rawStrings.append(str);
}

void TestFieldParser::testErrors()
{
    QCOMPARE(m_fieldParser->parse("test{"), false);
    QCOMPARE(m_fieldParser->parse("test{a"), false);
    QCOMPARE(m_fieldParser->parse("test{{test}"), false);
    QCOMPARE(m_fieldParser->parse("}"), false);
}

void TestFieldParser::testOuterField()
{
    QVERIFY(m_fieldParser->parse("{start}xyz{end}"));

    QCOMPARE(m_fields.size(), 2);
    QCOMPARE(m_fields[0], QString("start"));
    QCOMPARE(m_fields[1], QString("end"));

    QCOMPARE(m_rawStrings.size(), 1);
    QCOMPARE(m_rawStrings[0], QString("xyz"));
}

void TestFieldParser::testOuterRaw()
{
    QVERIFY(m_fieldParser->parse("start{xyz}end"));

    QCOMPARE(m_fields.size(), 1);
    QCOMPARE(m_fields[0], QString("xyz"));

    QCOMPARE(m_rawStrings.size(), 2);
    QCOMPARE(m_rawStrings[0], QString("start"));
    QCOMPARE(m_rawStrings[1], QString("end"));
}

void TestFieldParser::testEscape()
{
    QVERIFY(m_fieldParser->parse("{{}{{}{abc}{}}"));

    QCOMPARE(m_fields.size(), 1);
    QCOMPARE(m_fields[0], QString("abc"));

    QCOMPARE(m_rawStrings.size(), 3);
    QCOMPARE(m_rawStrings[0], QString("{"));
    QCOMPARE(m_rawStrings[1], QString("{"));
    QCOMPARE(m_rawStrings[2], QString("}"));
}

QTEST_GUILESS_MAIN(TestFieldParser)
