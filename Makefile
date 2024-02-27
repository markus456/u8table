all: table

table: table.cc table.hh
	c++ -DTABLE_FORMAT_FROM_ENV=1 -g table.cc -o table

clean:
	rm -f table
