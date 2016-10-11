# longest_path
Here are the implementations of algorithms solving Longest Path problem.
Gradient descent, Metropolis and Simulated Annealing are implemented.
## Algorithms analysis
Algorithms results are represented on this graph. On horizontal line there are different 100-vertices graphs and on the vertical one there are average results of each algorithm working on this graph. As we can see Gradient descent algorithm gives us the worst result (the blue line is usually below two others). Metropolis and Simulation annealing are pretty much the same. This tendence is evident because Gradient descent can never go away from local minima (maxima in this case). And for Metropolis algorithm there is always a probability to escape local extrema.


![](https://github.com/topshik/longest_path/blob/master/gr1.jpg)
