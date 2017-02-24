setenv OS_TYPE `uname -s`

if ( Darwin == ${OS_TYPE} ) then
    alias Top 'top -l1'
else
    alias Top 'top -d 1 -n 1'
endif

if      ( ${OS_TYPE} == HP-UX ) then
    alias free '(set file=/tmp/${USER}.$$;Top -f $file;cat $file;rm -f $file)'
else if ( ${OS_TYPE} != Linux ) then
    alias free Top
endif
setenv memory  `free |& awk '/Mem:/ {print $2}' | tr K k`

if (-X limit) then
    if { ( limit | grep ^datasize  > /dev/null ) } limit datasize ${memory}
endif
