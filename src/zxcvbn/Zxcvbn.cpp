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

#include "Zxcvbn.h"

#include <QScopedArrayPointer>

#include "RepeatMatcher.h"
#include "SequenceMatcher.h"

void Zxcvbn::passwordStrength(const QString& password, const QStringList& userInputs)
{
    QStringList sanitizedUserInputs;

    Q_FOREACH (const QString& input, userInputs) {
        sanitizedUserInputs.append(input.toLower());
    }

    matches(password);

    // TODO: return password strength
}

QList<Zxcvbn::Match> Zxcvbn::matches(const QString& password)
{
    QList<Matcher*> matchers({
        new RepeatMatcher(),
        new SequenceMatcher()
    });

    QList<Match> matches;

    for (Matcher* matcher : matchers) {
        matches.append(matcher->match(password));
    }

    qDeleteAll(matchers);

    return matches;
}
