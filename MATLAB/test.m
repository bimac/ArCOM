clc
format long
a = ArCOMObject([],115200);

x = pi;

a.write(0)
disp('Requested pi as double:')
fprintf('EXPECTED: %1.20f\nRECEIVED: %1.20f\n\n',pi,a.read(1,'double'))

a.write(1)
disp('Requested pi as char array:')
fprintf('EXPECTED: %1.20f\nRECEIVED: %1.20f\n\n',pi,str2double(a.read(22,'char')))

a.write(0)
disp('Requested pi as double (cast to uint8):')
fprintf(['EXPECTED: %3d %3d %3d %3d %3d %3d %3d %3d\n' ...
         'RECEIVED: %3d %3d %3d %3d %3d %3d %3d %3d\n\n'],...
        typecast(pi,'uint8'),a.read(8,'uint8'))

a.write(2)
a.write(x,'double')
disp('Sent X as double, returned as double:')
fprintf('EXPECTED: %1.20f\nRECEIVED: %1.20f\n\n',x,a.read(1,'double'))

a.write(3)
a.write(x,'double')
disp('Sent X as double, returned as char array:')
fprintf('EXPECTED: %1.20f\nRECEIVED: %1.20f\n\n',x,str2double(a.read(22,'char')))

a.write(2)
a.write(x,'double')
disp('Sent X as double, returned as double (cast to uint8):')
fprintf(['EXPECTED: %3d %3d %3d %3d %3d %3d %3d %3d\n' ...
         'RECEIVED: %3d %3d %3d %3d %3d %3d %3d %3d\n\n'],...
        typecast(x,'uint8'),a.read(8,'uint8'))

a.close