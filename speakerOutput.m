%play different signals on different channels on a stereo jack

Fs = 44000;
t = 0:1/Fs:5;
fA = 1000;
fB = 2000;
sigA = cos(4*pi*fA*t);
sigB = cos(0.5*pi*fB*t);
sigAB = [sigA ; sigB];

