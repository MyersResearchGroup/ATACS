function retval = ite(i,t,e)
for j = 1:length(i)
  if (i(j))
    retval(j)=t(j);
  else
    retval(j)=e(j);
  endif
endfor
endfunction
   
