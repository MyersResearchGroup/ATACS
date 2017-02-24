###########################################################################
## System being modelled

RSIZE = 5;
REGIONS = 10;
oVc = 0:.5/(RSIZE*REGIONS-1):.5;
oIl = 0:.0011/(RSIZE*REGIONS-1):.0011;
[Il,Vc] = meshgrid(oIl, oVc);
C = 1e-12;
G = 4.13e-3; # No Oscillation
##G = 5e-3; # Oscillation
L = 1e-6;
Vin = .3;

function retval = h(x)
  r1 = (6.0105 * x.^3 .- 0.9917 * x.^2 .+ 0.0545 * x) .* \ 
    (x >= 0 & x < 0.055);
  r2 = (0.0692 * x.^3 .- 0.0421 * x.^2 .+ 0.004 * x .+ 8.9579e-4) .* \
    (x >= 0.055 & x < 0.35);
  r3 = (0.2634 * x.^3 .- 0.2765 * x.^2 .+ 0.0968 * x .- 0.0112) .* \
    (x >= 0.35 & x <= 0.50);
  retval = r1 .+ r2 .+ r3;
endfunction

dIl = 1/L * (-Vc .- Il/G + Vin);
dVc = 1/C * (-h(Vc) .+ Il);

## SIMPLE SIMPLE SIMPLE example
#dIl = -Vc;

###########################################################################


## Calculates a matrix containing the length of vectors given n matrixes 
## where each matrix is a set of data points in one dimension.
function result = calcL(varargin)
  result = varargin{1} * 0;
  for i = 1:length(varargin)
    result = result .+ varargin{i} .^2;
  endfor
  result = sqrt(result) .+ 1;
endfunction


## Currently only works for two-dimensional matrix... Issue is in counting 
## the number of elements in the amtrix as marked below.
## Given an n-dimensional matrix, calculates lm, the vector length variation.
function result = lm(L)
  biggest = max(L);
  smallest = min(L);
  while (!is_scalar(biggest)) 
    biggest = max(biggest);
    smallest = min(smallest);
  endwhile 
  if (biggest == 0)
    result = 0;
  else
    result = (1 - smallest/biggest) * size(L, 1) * size(L, 2); #2d problem here
  endif
endfunction

## Given an n-dimensional matrix, calculates am, the maximum angle difference.
function result = am(L, varargin)
  biggest = -inf;
  smallest = inf;
  for i = 1:length(varargin)
    temp = abs(varargin{i}) ./ abs(L);
    b = temp;
    s = temp;
    while (!is_scalar(b))
      b = max(b);
    endwhile
    while (!is_scalar(s))
      s = min(s);
    endwhile
    if (b > biggest)
      biggest = b;
    endif
    if (s < smallest)
      smallest = s;
    endif
  endfor
  #biggest
  #smallest
  result = (biggest - smallest) * size(L, 1) * size(L, 2); #2d problem here;
endfunction

## Only works for two dimensions. can accept more than one dimension but won't
## do anything useful.  Has a problem slicing up the N regions correctly still.
## perhaps the varname{:} syntax will help... I think I got it... see notes 
## for 9/28/03.
function cost = cost(regions, L, varargin) 
  #############################################
  ## These variables need to be fine-tuned
  wml = 1;  # Weighting to use for "l" mean
  wvl = 0;  # Weighting to use for "l" std dev
  wma = 0;  # Weighting to use for "a" mean
  wva = 0;  # Weighting to use for "a" std dev
  wn = 0;   # Weighting to use for # of regions
  #############################################

  n = 0;   # Total # of regions
  for i = 1:length(regions{1})
    for j = 1:length(regions{2})
      n++;
      newL = L(regions{1}{i}, regions{2}{j});
      for k = 1:length(varargin)
        newvarargin{k} = varargin{k}(regions{1}{i}, regions{2}{j});
      endfor
      #a_results(n) = am(newL, newvarargin{:});
      l_results(n) = lm(newL);
    endfor
  endfor
  #a_results
  #l_results
  #n
  #cost =  wml * mean(l_results) + wvl * std(l_results) + \
  #        wn * n;
          #wma * mean(a_results) + wva * std(a_results) + \
  ###### Try #2
  #max(l_results)
  #min(l_results)
  #cost = abs(max(l_results) - min(l_results))
  ###### Try #1
  cost = sum(l_results);
  if (isnan(cost))
    printf("Bad cost calculated\n");
    a_results
    l_results
  endif
endfunction

function result = dupCellArray(ca)
  for i = 1:length(ca)
    result{i} = ca{i};
  endfor
endfunction

function newRegions = addDivision(regions, L, varargin)
  ## Iterate through all possible divisions in all dimensions and find 
  ## the division which will cost the least.
  bestDim = 0;
  bestk = 0;
  bestDimRegions = [];
  minCost = inf;
  for i = 1:length(regions)
    printf("Exploring dimension %i\n", i);
    oldRegions = dupCellArray(regions{i});
    regions{i}{length(regions{i})+1} = inf:inf;
    for j = 1:length(regions{i})-1
      range = regions{i}{j};
      first = range(1);
      last = range(length(range));
      if (first == last)
        continue;
      endif
      #oldRegions
      for k = first+1:last 
        regions{i}{j} = first:k-1;
        regions{i}{length(regions{i})} = k:last;
        c = cost(regions, L, varargin{:});
        #printf("  Cut at %i results in cost %f\n", k, c);
        #regions
        #pause;
        if c < minCost
          bestk = k;
          minCost = c;
          bestDim = i;
          bestDimRegions = dupCellArray(regions{i});
        endif
      endfor
      regions{i}{j} = range;
    endfor
    regions{i} = oldRegions;
  endfor
  if (isinf(minCost) || isnan(minCost))
    printf("No valid divisions founds.  Indicates serious error.\n");
    exit;
  endif
  printf("Adding division at row %i in dimension %i\n", bestk, bestDim);
  newRegions = regions;
  newRegions{bestDim} = bestDimRegions;
  minCost
endfunction

function cnt = countRegions(regions)
  cnt = 1;
  for i = 1:length(regions)
    cnt = cnt * length(regions{i});
  endfor
endfunction

function bestRegions = divide(p, varargin)
  ## Calculate L initially.
  L = calcL(varargin{:});
  ## Initially, the regions will be as large as possible
  ## for each dimension.
  for i = 1:length(varargin)
    regions{i} = {(1:size(L, i))};
  endfor
  while countRegions(regions) < p
    regions = addDivision(regions, L, varargin{:});
    printf("Total number of regions: %i\n", countRegions(regions));
  endwhile
  bestRegions = regions;
endfunction

function bestRegions = evenDivide(divs, varargin)
  ## Calculate L initially.
  L = calcL(varargin{:});
  for i = 1:length(varargin)
    regions{i} = {(1:size(L, i))};
  endfor
  for i = 1:length(regions)
    n = length(regions{i})
    for j = 1:n
      range = regions{i}{j};
      first = range(1);
      last = range(length(range));
      sep = (length(regions{i}{j}))/divs
      last = round(sep);
      regions{i}{j} = first:last;
      for k = 2:divs
        first = last+1;
        last = round(k*sep);
        regions{i}{length(regions{i})+1} = first:last;
      endfor
      printf("Total number of regions: %i\n", countRegions(regions));
    endfor
  endfor
  bestRegions = regions;
endfunction


## Only works for two dimensions.  
function fprime = applyRegions(regions, f)
  for i = 1:length(regions{1})
    for j = 1:length(regions{2})
      average = sum(sum(f(regions{1}{i}, regions{2}{j})))/ \
          (length(regions{1}{i}) * length(regions{2}{j}));
      fprime(regions{1}{i}, regions{2}{j}) = average;
    endfor
  endfor  
endfunction

## Given a set of regions, sorts it into acceding order.
function r = sortRegions(r) 
  for i = 1:length(r)
    for j = i+1:length(r)
      if (r{i}(1) > r{j}(1))
        temp = r{i};
        r{i} = r{j};
        r{j} = temp;
      endif
    endfor
  endfor
endfunction

## Print regions
## Only works for two dimensions.
function printRegions(regions, oIl, oVc, dIl, dVc)
  fp = fopen("foo.txt", "w", "ieee-le");
  fprintf(fp, "Vcd Vcu Ild Ilu dVc dIl\n");
  for i = 1:length(regions{1})
    for j = 1:length(regions{2})
      fprintf(fp, "%g %g %g %g ",  
              round(oVc(regions{1}{i}(1))*1e+4),
              round(oVc(regions{1}{i}(length(regions{1}{i})))*1e+4),
              round(oIl(regions{2}{j}(1))*1e+6),
              round(oIl(regions{2}{j}(length(regions{2}{j})))*1e+6));
                
      dIlAve = sum(sum(dIl(regions{1}{i}, regions{2}{j}))) / \
          (length(regions{1}{i}) * length(regions{2}{j}));
      dIlAve = round(dIlAve*1e-10*1e+6);
      dVcAve = sum(sum(dVc(regions{1}{i}, regions{2}{j}))) / \
          (length(regions{1}{i}) * length(regions{2}{j}));
      dVcAve = round(dVcAve*1e-10*1e+4);

      fprintf(fp, "%g %g\n", dVcAve, dIlAve);
    endfor
  endfor  
endfunction

r = evenDivide(4, dIl, dVc);
#r = divide(16, dIl, dVc);
r{1} = sortRegions(r{1});
r{2} = sortRegions(r{2});
r
printRegions(r, oIl, oVc, dIl, dVc);

## For Printer
gset terminal postscript eps
gset output "plot.eps"

#mesh (Vc, Il, dVc)
gset nokey
xlabel("Vc")
ylabel("Il")
zlabel("dIl/dt")

#mesh (Vc, Il, applyRegions(r, dVc))
#mesh (Vc, Il, dIl)
mesh (Vc, Il, applyRegions(r, dIl))
#mesh (Vc, Il, calcL(dIl, dVc))

#mesh(Vc, Il, dVc)



#mesh(Vc, Il, piecewise(dIl,RSIZE));

#pause;

#mesh(Vc, Il, piecewise(dVc,RSIZE));

#pause;

