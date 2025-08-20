# rprint

rprint (or Re1ssPrint) is a custom print function for C. Its main purpose was to create a more efficient alternative to the standard library's printf. 

## Why ?

First of all, it's fun. I just love learning new things.<br>
Secondly i wanted to test something, is **printf** the best ?<br>

## Conclusion

* Tested CPU : i7-7700K
* Tested Terminal : kitty 0.42.2
* Tested Shell : bash 5.3.3
* Tested WM : Hyprland 0.50.1 (Wayland)
* Total Prints (for both) : 100.000

I tried to run it on tty too... But the performance wasn't stable.

The lowest i saw was 0.78x but i didn't captured it.<br>
This is the lowest one i captured 0.80x
```
Standard printf: 0.199075 seconds (1990.75 ns/call)
Custom printf: 0.158590 seconds (1585.90 ns/call)
```
