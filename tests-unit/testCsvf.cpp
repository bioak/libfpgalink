/*
 * Copyright (C) 2009-2012 Chris McClelland
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <UnitTest++.h>
#include <makestuff.h>
#include <libbuffer.h>
#include "../private.h"
#include "../xsvf.h"
#include "../csvfreader.h"

static void testRoundTrip(const char *xsvfFile, uint32 expectedMaxBufSize) {
	Buffer csvfBuf, uncompressedBuf;
	BufferStatus bStatus;
	uint32 maxBufSize;
	FLStatus fStatus;
	bStatus = bufInitialise(&csvfBuf, 1024, 0x00, NULL);
	CHECK(bStatus == BUF_SUCCESS);
	bStatus = bufInitialise(&uncompressedBuf, 1024, 0x00, NULL);
	CHECK(bStatus == BUF_SUCCESS);

	// Load XSVF, convert to uncompressed CSVF
	fStatus = flLoadXsvfAndConvertToCsvf(xsvfFile, &csvfBuf, &maxBufSize, NULL);
	CHECK(fStatus == FL_SUCCESS);
	CHECK_EQUAL(expectedMaxBufSize, maxBufSize);

	// Make a copy of the uncompressed buffer
	bStatus = bufDeepCopy(&uncompressedBuf, &csvfBuf, NULL);
	CHECK(bStatus == BUF_SUCCESS);

	// Compress the CSVF buffer
	fStatus = flCompressCsvf(&csvfBuf, NULL);
	CHECK(fStatus == FL_SUCCESS);

	// Make a reader to iterate over the compressed data
	Context cp;
	uint8 thisByte;
	thisByte = csvfInitReader(&cp, csvfBuf.data, true);
	CHECK(thisByte == 0x00);

	// Uncompress the compressed data into the reconstituteBuf
	Buffer reconstituteBuf;
	bStatus = bufInitialise(&reconstituteBuf, uncompressedBuf.length, 0x00, NULL);
	CHECK(bStatus == BUF_SUCCESS);
	for ( uint32 i = 0; i < uncompressedBuf.length; i++ ) {
		bStatus = bufAppendByte(&reconstituteBuf, csvfGetByte(&cp), NULL);
		CHECK(bStatus == BUF_SUCCESS);
	}

	// Make sure the result of the compress-uncompress operation is the same as the original data
	CHECK_EQUAL(uncompressedBuf.length, reconstituteBuf.length);
	CHECK_ARRAY_EQUAL(uncompressedBuf.data, reconstituteBuf.data, uncompressedBuf.length);

	//bStatus = bufWriteBinaryFile(&uncompressedBuf, "uncompressed.dat", 0x00000000, uncompressedBuf.length, NULL);
	//CHECK(bStatus == BUF_SUCCESS);
	//bStatus = bufWriteBinaryFile(&reconstituteBuf, "reconstitute.dat", 0x00000000, reconstituteBuf.length, NULL);
	//CHECK(bStatus == BUF_SUCCESS);

	bufDestroy(&reconstituteBuf);
	bufDestroy(&uncompressedBuf);
	bufDestroy(&csvfBuf);
}

TEST(FPGALink_testRoundTrip) {
	// for i in $(find ../hdl/ -name "*.xsvf"); do export DEPTH=$(../xsvf2csvf/linux.x86_64/rel/xsvf2csvf $i /tmp/foo.csvf | awk '{print $NF}'); printf "\ttestRoundTrip(\"$i\", ${DEPTH}U);\n"; done >> testCsvf.cpp
	testRoundTrip("../hdl/apps/makestuff/swled/cksum/verilog/xsvf/fx2all-nexys2-1200.xsvf", 5U);
	testRoundTrip("../hdl/apps/makestuff/swled/cksum/verilog/xsvf/fx2all-xylo-l.xsvf", 4U);
	testRoundTrip("../hdl/apps/makestuff/swled/cksum/verilog/xsvf/epp-nexys2-1200.xsvf", 5U);
	testRoundTrip("../hdl/apps/makestuff/swled/cksum/verilog/xsvf/fx2all-nexys3.xsvf", 4U);
	testRoundTrip("../hdl/apps/makestuff/swled/cksum/verilog/xsvf/fx2all-atlys.xsvf", 4U);
	testRoundTrip("../hdl/apps/makestuff/swled/cksum/verilog/xsvf/fx2min-lx9.xsvf", 4U);
	testRoundTrip("../hdl/apps/makestuff/swled/cksum/verilog/xsvf/fx2all-s3board.xsvf", 5U);
	testRoundTrip("../hdl/apps/makestuff/swled/cksum/verilog/xsvf/fx2all-nexys2-500.xsvf", 5U);
	testRoundTrip("../hdl/apps/makestuff/swled/cksum/vhdl/xsvf/fx2all-nexys2-1200.xsvf", 5U);
	testRoundTrip("../hdl/apps/makestuff/swled/cksum/vhdl/xsvf/fx2all-xylo-l.xsvf", 4U);
	testRoundTrip("../hdl/apps/makestuff/swled/cksum/vhdl/xsvf/epp-nexys2-1200.xsvf", 5U);
	testRoundTrip("../hdl/apps/makestuff/swled/cksum/vhdl/xsvf/fx2all-nexys3.xsvf", 4U);
	testRoundTrip("../hdl/apps/makestuff/swled/cksum/vhdl/xsvf/fx2all-atlys.xsvf", 4U);
	testRoundTrip("../hdl/apps/makestuff/swled/cksum/vhdl/xsvf/fx2min-lx9.xsvf", 4U);
	testRoundTrip("../hdl/apps/makestuff/swled/cksum/vhdl/xsvf/fx2all-s3board.xsvf", 5U);
	testRoundTrip("../hdl/apps/makestuff/swled/cksum/vhdl/xsvf/fx2all-nexys2-500.xsvf", 5U);
	testRoundTrip("../hdl/apps/makestuff/swled/fifo/verilog/xsvf/fx2all-nexys2-1200.xsvf", 5U);
	testRoundTrip("../hdl/apps/makestuff/swled/fifo/verilog/xsvf/fx2all-xylo-l.xsvf", 4U);
	testRoundTrip("../hdl/apps/makestuff/swled/fifo/verilog/xsvf/epp-nexys2-1200.xsvf", 5U);
	testRoundTrip("../hdl/apps/makestuff/swled/fifo/verilog/xsvf/fx2all-nexys3.xsvf", 4U);
	testRoundTrip("../hdl/apps/makestuff/swled/fifo/verilog/xsvf/fx2all-atlys.xsvf", 4U);
	testRoundTrip("../hdl/apps/makestuff/swled/fifo/verilog/xsvf/fx2min-lx9.xsvf", 4U);
	testRoundTrip("../hdl/apps/makestuff/swled/fifo/verilog/xsvf/fx2all-s3board.xsvf", 5U);
	testRoundTrip("../hdl/apps/makestuff/swled/fifo/verilog/xsvf/fx2all-nexys2-500.xsvf", 5U);
	testRoundTrip("../hdl/apps/makestuff/swled/fifo/vhdl/xsvf/fx2all-nexys2-1200.xsvf", 5U);
	testRoundTrip("../hdl/apps/makestuff/swled/fifo/vhdl/xsvf/fx2all-xylo-l.xsvf", 4U);
	testRoundTrip("../hdl/apps/makestuff/swled/fifo/vhdl/xsvf/epp-nexys2-1200.xsvf", 5U);
	testRoundTrip("../hdl/apps/makestuff/swled/fifo/vhdl/xsvf/fx2all-nexys3.xsvf", 4U);
	testRoundTrip("../hdl/apps/makestuff/swled/fifo/vhdl/xsvf/fx2all-atlys.xsvf", 4U);
	testRoundTrip("../hdl/apps/makestuff/swled/fifo/vhdl/xsvf/fx2min-lx9.xsvf", 4U);
	testRoundTrip("../hdl/apps/makestuff/swled/fifo/vhdl/xsvf/fx2all-s3board.xsvf", 5U);
	testRoundTrip("../hdl/apps/makestuff/swled/fifo/vhdl/xsvf/fx2all-nexys2-500.xsvf", 5U);
}
