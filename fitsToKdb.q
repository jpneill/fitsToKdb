-1"Loading fits lib extension functions from fitsToKdb.so.";

.astro.listHDUs:`fitsToKdb 2:(`listHDUs;1);
.astro.getFitsRowCount:`fitsToKdb 2:(`num_rows;2);
.astro.getFitsColCount:`fitsToKdb 2:(`num_cols;2);
.astro.getFitsColNames:`fitsToKdb 2:(`cols;2);
.astro.getFitsColType:`fitsToKdb 2:(`getColType;3);
.astro.readLongCol:`fitsToKdb 2:(`readLongCol;4);
.astro.readDoubleCol:`fitsToKdb 2:(`readDoubleCol;4);
.astro.readIntCol:`fitsToKdb 2:(`readIntCol;4);
.astro.readSymCol:`fitsToKdb 2:(`readSymCol;4);

-1"Loading fits q functions.";

///
// .astro.createTab creates a table of astronomy data from a given fits file, the hdu to reference and the number of lines to load
// @param f fits file - symbol
// @param n number of rows to load into kdb - long/int
// @param hn number of the hdu in the fits file f that the data to be retrieved is stored under - long/int
// example load 500 rows from table in hdu 2 in file example.fits
// q).astro.createTab[`:example.fits;500;2]
.astro.createTab:{[f;n;hn]
  // Get number of cols in table
  nc:.astro.getFitsColCount[f;hn];
  // If n=0 Get number of rows in table.
  if[n=0;n:.astro.getFitsRowCount[f;hn]];
  // Get list of column names in table
  c:.astro.getFitsColNames[f;hn];
  // Get list of column types in table
  t:.astro.getFitsColType[f;;hn]each c;
  // Get int cols
  -1"Retrieving int cols from ",string f;
  iCols:iCols!.astro.readIntCol[f;;n;hn]each iCols:c where t=`I;
  // Get long cols
  -1"Retrieving long cols from ",string f;
  jCols:jCols!.astro.readLongCol[f;;n;hn]each jCols:c where t=`J;
  // Get float cols
  -1"Retrieving float cols from ",string f;
  fCols:fCols!.astro.readDoubleCol[f;;n;hn]each fCols:c where t in `E`D;
  // Get symbols cols - treat all remaining columns as symbols
  -1"Retrieving symbol cols from ",string f;
  sCols:sCols!.astro.readSymCol[f;;n;hn]each sCols:c where not t in `I`J`E`D;
  // Combine all into table
  flip raze (iCols;jCols;fCols;sCols)
 }