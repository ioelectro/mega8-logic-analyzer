# Simple Logic Analyzer 

Sampling one GPIO 7200 time by timer in 100us of period (you can change that, but it's good for me!) and save to buffer for sending to PC by UART and Plot chart using [SerialPlot](https://hackaday.io/project/5334-serialplot-realtime-plotting-software) app.

Press button for start buffering, and another time for sending to PC, BR is 9600, you can speed up that.

I use *CodeVisionAVR* for compiling that, with small changes you can use GCC, I use big area of RAM for Variable in Config. 


![Screenshot 2021-05-11 104434](https://user-images.githubusercontent.com/64005694/117767245-eb979700-b245-11eb-85df-b58678b046cb.jpg)

