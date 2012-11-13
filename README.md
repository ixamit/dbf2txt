dbf2txt 
=======
Query DBF file exporting  data fields to different formats.
Follow my examples (and try) with the extra distribution file TEST.DBF 

INSTALL
-------
    % VERSION=0.1; 
    % PACKAGE=dbf2txt-$VERSION
    % tar -zxvf $PACKAGE.tar.gz
    % cd $PACKAGE
    % ./configure [1]
    % make 
    $ make install
[1] by default the package will install in:
/usr/local/bin/dbf2txt [binary]
/usr/local/lib/dbf2txt/ [shared & dynamic lib]

See the options help
--------------------
    % dbf2txt --help
    Usage: dbf2txt [options]... 

    Help and Configuration:
    -h, --help                             Print this help
    Query (SQL Style):
    -d, --desc="ARCHIVE.DBF"               Describe an DBF 
    -s, --select="FIELD..."                Select data field(s)
    -f, --from="ARCHIVE.DBF"               Archive DBF
    -w, --where="CONDITION..."             Condition filter clause
    -o, --order="FIELD..."                 Sort field(s)
    Output (optional plugins):
    -x, --xdump="NAME"                     Dump to plugin output. Use `-x help` for a list of loaded plugins
Describe TEST.DBF 
-----------------
    % dbf2txt --desc test.dbf 
    +------------------------------------------+
    | DataBase type: 0x03   Update: 2012-10-03 |
    | FoxBASE+/dBASE III plus, no memo         |
    +------------+-------------+---------------+
    | Number of  | Position of | Len of Record |
    | Records    | 1st  Record | w delete flag |
    +------------+-------------+---------------+
    +------------+-------------+-------+-------+
    | Field Name | Type        | Len   | Dec   |
    +------------+-------------+-------+-------+
    | ID         | N Numeric   |    10 |     0 |
    | DES        | C Character |     5 |     0 |
    | PRZ        | N Numeric   |    10 |     2 |
    +------------+-------------+-------+-------+
Show all fields with no-order
-----------------------------
    % dbf2txt --from test.dbf
    +----+------+-------+
    | ID | DES  | PRZ   |
    +----+------+-------+
    |  1 | Dw1A | 18.47 |
    |  2 | Fu2A | 25.28 |
    |  3 | Cx1A | 13.25 |
    |  4 | Gu2A | 28.97 |
    |  5 | Lp4B | 54.95 |
    |  6 | By0A |  8.57 |
    |  7 | Fu2A | 26.07 |
    |  8 | Dw1A | 15.60 |
    |  9 | Pl6C | 73.01 |
    | 10 | Nn5B | 64.43 |
    | 11 | Gt2A | 31.66 |
    | 12 | Jq4B | 47.27 |
    | 13 | Nm5B | 66.34 |
    | 14 | Gt2A | 30.33 |
    | 15 | Pk6C | 75.25 |
    | 16 | Gt2A | 32.11 |
    | 17 | Nn5B | 62.11 |
    | 18 | Cx1A | 13.95 |
    | 19 | Kq4B | 48.41 |
    +----+------+-------+

Show SELECT fields with no-order
--------------------------------
    % dbf2txt --select "PRZ DES" --from test.dbf
    +-------+------+
    | PRZ   | DES  |
    +-------+------+
    | 18.47 | Dw1A |
    | 25.28 | Fu2A |
    | 13.25 | Cx1A |
    | 28.97 | Gu2A |
    | 54.95 | Lp4B |
    |  8.57 | By0A |
    | 26.07 | Fu2A |
    | 15.60 | Dw1A |
    | 73.01 | Pl6C |
    | 64.43 | Nn5B |
    | 31.66 | Gt2A |
    | 47.27 | Jq4B |
    | 66.34 | Nm5B |
    | 30.33 | Gt2A |
    | 75.25 | Pk6C |
    | 32.11 | Gt2A |
    | 62.11 | Nn5B |
    | 13.95 | Cx1A |
    | 48.41 | Kq4B |
    +-------+------+

Show SELECT fields with condition filter clause
-----------------------------------------------
    % dbf2txt --select "PRZ DES" --from test.dbf --where "PRZ>50"
    +-------+------+
    | PRZ   | DES  |
    +-------+------+
    | 54.95 | Lp4B |
    | 73.01 | Pl6C |
    | 64.43 | Nn5B |
    | 66.34 | Nm5B |
    | 75.25 | Pk6C |
    | 62.11 | Nn5B |
    +-------+------+
    
    % dbf2txt --select "PRZ DES" --from test.dbf --where "PRZ>50 and DES=N"
    +-------+------+
    | PRZ   | DES  |
    +-------+------+
    | 64.43 | Nn5B |
    | 66.34 | Nm5B |
    | 62.11 | Nn5B |
    +-------+------+

    % dbf2txt --select "PRZ DES" --from test.dbf --where "PRZ>50 and DES!=N"
    +-------+------+
    | PRZ   | DES  |
    +-------+------+
    | 54.95 | Lp4B |
    | 73.01 | Pl6C |
    | 75.25 | Pk6C |
    +-------+------+

Show with order
---------------
    % dbf2txt --select "PRZ DES" --from test.dbf --where "PRZ>50 and DES=N" --order "PRZ"
    +-------+------+
    | PRZ   | DES  |
    +-------+------+
    | 62.11 | Nn5B |
    | 64.43 | Nn5B |
    | 66.34 | Nm5B |
    +-------+------+
    
    % dbf2txt --from test.dbf --order "DES PRZ"
    +----+------+-------+
    | ID | DES  | PRZ   |
    +----+------+-------+
    |  6 | By0A |  8.57 |
    |  3 | Cx1A | 13.25 |
    | 18 | Cx1A | 13.95 |
    |  8 | Dw1A | 15.60 |
    |  1 | Dw1A | 18.47 |
    |  2 | Fu2A | 25.28 |
    |  7 | Fu2A | 26.07 |
    | 14 | Gt2A | 30.33 |
    | 11 | Gt2A | 31.66 |
    | 16 | Gt2A | 32.11 |
    |  4 | Gu2A | 28.97 |
    | 12 | Jq4B | 47.27 |
    | 19 | Kq4B | 48.41 |
    |  5 | Lp4B | 54.95 |
    | 13 | Nm5B | 66.34 |
    | 17 | Nn5B | 62.11 |
    | 10 | Nn5B | 64.43 |
    | 15 | Pk6C | 75.25 |
    |  9 | Pl6C | 73.01 |
    +----+------+-------+

Dump other formats with plugins
-------------------------------
    % dbf2txt --xdump HELP
    Available xdump plugins:
    name:   descriprion: [lib]
    CSV     Comma-Separated Value (CSV)
    SQL     Structured Query Language (SQL)
    XML     Extensible Markup Language (XML)

    % dbf2txt --select "id desc" --from test.dbf -x sql
    -- SQL dump created by dbf2txt version 0.1
    -- please report bugs at ixamit_at_gmail_dot_com
    --
    -- ----------------------------------------------
    --
    -- Table structure for table `test`
    --
    
    DROP TABLE IF EXISTS `test`;
    CREATE TABLE `test` (
      `ID` int(10),
      `DES` varchar(5) COLLATE utf8_unicode_ci
    
      /* Modify and or/Add KEYS */
      # PRIMARY KEY (`ID`)
    ) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
    
    --
    -- Dumping data from table `test`
    --
    
    LOCK TABLES `test` WRITE;
    INSERT INTO `test` VALUES 
    (1,"Dw1A"),
    (2,"Fu2A"),
    (3,"Cx1A"),
    (4,"Gu2A"),
    (5,"Lp4B"),
    (6,"By0A"),
    (7,"Fu2A"),
    (8,"Dw1A"),
    (9,"Pl6C"),
    (10,"Nn5B"),
    (11,"Gt2A"),
    (12,"Jq4B"),
    (13,"Nm5B"),
    (14,"Gt2A"),
    (15,"Pk6C"),
    (16,"Gt2A"),
    (17,"Nn5B"),
    (18,"Cx1A"),
    (19,"Kq4B");
    UNLOCK TABLES;
    
    % dbf2txt --from test.dbf -x sql | mysql -u user -p database
    Enter password: **********
    %

Writing xyz plugin
------------------
*** MUST Install: autoconf, automake, autotools ***

    % cd $PACKAGE
    % vi configure.in
      (add "plugin/xyz/Makefile" to macro AC_OUTPUT)
    % vi plugin/Makefile.am
      (add xyz to SUBDIRS)
    % cp -R plugin/foo plugin/xyz
    % mv plugin/xyz/foo.c plugin/xyz/xyz.c 
    % vi plugin/xyz/Makefile.am
      (change any foo to xyz)
    % vi plugin/xyz/xyz.c 
      (change any FOO to XYZ)

    % autoreconf
    % make clean
    % make 
    $ make install

your plugin "plugin/xyz.c" MUST HAVE:
* all #Include *.h as plugin/foo.c
* function plug_init (PLUGIN *foo) like:
      int plugin_init (PLUGIN *xyz)
      { 
        xyz->name="XYZ";
        xyz->desc="Xor Yeti Zanzibar (XYZ)";
        xyz->vers="0.1";

        return 0;
      }
field name should be less 4 uppercase chars 
field desc is a short description
field vers is the version 
* functions XYZ_HEAD, XYZ_TAIL, XYZ_BODY 
  (Note:the above prefix is refered to field xyz->name not the filename)
  void XYZ_HEAD (DBF *dbf) 
  {
     // Header
	 int i;
     FOREACH_SELECT(i)
     {
        printf ("%s\n",DBFIELD_NAME(i));
     }
  }
  void XYZ_TAIL (DBF *dbf) 
  {
     // TAIL
  }
  void XYZ_BODY (DBF *dbf,int RecNumber)
  {
     // BODY
     int i;
     FOREACH_SELECT(i)
     {
        DBFIELD_LOAD_CONTENTS(i);
        printf ("%s\n",DBFIELD_CONTENTS);
     }
  }

Bench Test
----------
Dumping fields to /dev/null: 
* Filesize: 4Mb 
* N.Record: 10,000
* Ordered Field: 25
Real  0m0.400s
User  0m0.336s
sys   0m0.064s


