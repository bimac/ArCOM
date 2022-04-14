clc
a = ArCOMObject([],[],'java');

a.write(4)
disp('Requested pi as float:')
tmp = a.read(1,'single');
fprintf('EXPECTED: %1.15f\nRECEIVED: %1.15f\nDIFF:     %1.15f\n\n',...
    single(pi),tmp,abs(single(pi)-tmp))

a.write(0)
disp('Requested pi as double:')
tmp = a.read(1,'double');
fprintf('EXPECTED: %1.15f\nRECEIVED: %1.15f\nDIFF:     %1.15f\n\n',...
    pi,tmp,abs(pi-tmp))

a.write(1)
disp('Requested pi as char array:')
fprintf('EXPECTED: %1.15f\nRECEIVED: %s\n\n',...
    pi,a.read(18,'char'))

a.write(0)
disp('Requested pi as double (cast to uint8):')
tmp = a.read(8,'uint8');
fprintf(['EXPECTED: %3d %3d %3d %3d %3d %3d %3d %3d\n' ...
         'RECEIVED: %3d %3d %3d %3d %3d %3d %3d %3d\n' ...
         'DIFF:     %3d %3d %3d %3d %3d %3d %3d %3d\n\n'],...
        typecast(pi,'uint8'),tmp,abs(tmp-typecast(pi,'uint8')))

a.write(2)
a.write(pi,'double')
disp('Sent pi as double, returned as double:')
tmp = a.read(1,'double');
fprintf('EXPECTED: %1.15f\nRECEIVED: %1.15f\nDIFF:     %1.15f\n\n',...
        pi,tmp,abs(pi-tmp))

a.write(3)
a.write(pi,'double')
disp('Sent pi as double, returned as char array:')
fprintf('EXPECTED: %1.15f\nRECEIVED: %s\n\n',...
        pi,a.read(18,'char'))

a.write(2)
a.write(pi,'double')
disp('Sent pi as double, returned as double (cast to uint8):')
tmp = a.read(8,'uint8');
fprintf(['EXPECTED: %3d %3d %3d %3d %3d %3d %3d %3d\n' ...
         'RECEIVED: %3d %3d %3d %3d %3d %3d %3d %3d\n' ...
         'DIFF:     %3d %3d %3d %3d %3d %3d %3d %3d\n\n'],...
        typecast(pi,'uint8'),tmp,abs(tmp-typecast(pi,'uint8')))

a.close