// RangeRotator – A LibreOffice Calc extension to rotate the selection
// Copyright (C) 2026  Neil Roberts
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "rangerotator.hxx"

#include <com/sun/star/container/XEnumeration.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/sheet/XCellRangeAddressable.hpp>
#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>
#include <com/sun/star/table/XCellRange.hpp>
#include <com/sun/star/text/XTextRange.hpp>

namespace
{
void swapCells(const css::uno::Reference<css::table::XCell>& xCellA,
               const css::uno::Reference<css::table::XCell>& xCellB)
{
    const css::uno::Reference<css::text::XTextRange> xTextA(xCellA, css::uno::UNO_QUERY);
    const css::uno::Reference<css::text::XTextRange> xTextB(xCellB, css::uno::UNO_QUERY);

    if (!xTextA.is() || !xTextB.is())
        return;

    rtl::OUString sTemp = xTextA->getString();
    xTextA->setString(xTextB->getString());
    xTextB->setString(sTemp);
}

void rotateRange(const css::uno::Reference<css::table::XCellRange>& xCellRange, sal_Int32 nWidth,
                 sal_Int32 nHeight)
{
    sal_Int32 nYMiddle = nHeight / 2;

    for (sal_Int32 y = 0; y < nYMiddle; ++y)
    {
        for (sal_Int32 x = 0; x < nWidth; ++x)
        {
            swapCells(xCellRange->getCellByPosition(nWidth - 1 - x, nHeight - 1 - y),
                      xCellRange->getCellByPosition(x, y));
        }
    }

    // If there is an odd number of rows then the middle row needs to be rotated on its own because
    // we only want to iterate over half the columns.
    if ((nHeight & 1) != 0)
    {
        for (sal_Int32 x = 0, nXMiddle = nWidth / 2; x < nXMiddle; ++x)
        {
            swapCells(xCellRange->getCellByPosition(nWidth - 1 - x, nYMiddle),
                      xCellRange->getCellByPosition(x, nYMiddle));
        }
    }
}

void rotateRange(const css::uno::Reference<css::table::XCellRange>& xCellRange)
{
    css::uno::Reference<css::sheet::XCellRangeAddressable> xAddressable(xCellRange,
                                                                        css::uno::UNO_QUERY);

    if (!xAddressable.is())
        return;

    css::table::CellRangeAddress sAddress = xAddressable->getRangeAddress();
    sal_Int32 nWidth = sAddress.EndColumn - sAddress.StartColumn + 1;
    sal_Int32 nHeight = sAddress.EndRow - sAddress.StartRow + 1;
    rotateRange(xCellRange, nWidth, nHeight);
}
}

namespace uk::co::busydoingnothing::rangerotator
{
void rotate(const css::uno::Reference<css::sheet::XSpreadsheetDocument>& xDocument)
{
    css::uno::Reference<css::frame::XModel> xModel(xDocument, css::uno::UNO_QUERY);

    if (!xModel.is())
        return;

    css::uno::Reference<css::lang::XServiceInfo> xSelectionInfo(xModel->getCurrentSelection(),
                                                                css::uno::UNO_QUERY);

    if (xSelectionInfo.is()
        && xSelectionInfo->supportsService("com.sun.star.sheet.SheetCellRanges"))
    {
        // The selection has multiple ranges
        css::uno::Reference<css::container::XEnumerationAccess> xEnumAccess(xSelectionInfo,
                                                                            css::uno::UNO_QUERY);

        if (!xEnumAccess.is())
            return;

        for (css::uno::Reference<css::container::XEnumeration> xEnum
             = xEnumAccess->createEnumeration();
             xEnum->hasMoreElements();)
        {
            css::uno::Reference<css::table::XCellRange> xCellRange;

            if (!(xEnum->nextElement() >>= xCellRange) || !xCellRange.is())
                continue;

            rotateRange(xCellRange);
        }
    }
    else if (css::uno::Reference<css::table::XCellRange> xCellRange(xSelectionInfo,
                                                                    css::uno::UNO_QUERY);
             xCellRange.is())
    {
        // The selection is one simple range
        rotateRange(xCellRange);
    }
}
}
