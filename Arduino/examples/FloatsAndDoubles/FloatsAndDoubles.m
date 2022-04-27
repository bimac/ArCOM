a = ArCOMObject();

disp('Requesting pi as float:')
a.write(0)
tmp = a.read(1,'single');
fprintf('EXPECTED: %1.15f\nRECEIVED: %1.15f\nDIFF:     %1.15f\n\n',...
    single(pi),tmp,abs(single(pi)-tmp))

disp('Requesting pi as double:')
a.write(1)
tmp = a.read(1,'double');
fprintf('EXPECTED: %1.15f\nRECEIVED: %1.15f\nDIFF:     %1.15f\n\n',...
    pi,tmp,abs(pi-tmp))

disp('Requesting pi as char array:')
a.write(2)
fprintf('EXPECTED: %1.15f\nRECEIVED: %s\n\n',...
    pi,a.read(18,'char'))

disp('Requesting pi as double (cast to uint8):')
a.write(1)
tmp = a.read(8,'uint8');
fprintf(['EXPECTED: %3d %3d %3d %3d %3d %3d %3d %3d\n' ...
         'RECEIVED: %3d %3d %3d %3d %3d %3d %3d %3d\n' ...
         'DIFF:     %3d %3d %3d %3d %3d %3d %3d %3d\n\n'],...
        typecast(pi,'uint8'),tmp,abs(tmp-typecast(pi,'uint8')))

disp('Send pi as double, return as double:')
a.write(3)
a.write(pi,'double')
tmp = a.read(1,'double');
fprintf('EXPECTED: %1.15f\nRECEIVED: %1.15f\nDIFF:     %1.15f\n\n',...
        pi,tmp,abs(pi-tmp))

disp('Send pi as double, return as char array:')
a.write(4)
a.write(pi,'double')
fprintf('EXPECTED: %1.15f\nRECEIVED: %s\n\n',...
        pi,a.read(18,'char'))

disp('Send pi as double, return as double (cast to uint8):')
a.write(3)
a.write(pi,'double')
tmp = a.read(8,'uint8');
fprintf(['EXPECTED: %3d %3d %3d %3d %3d %3d %3d %3d\n' ...
         'RECEIVED: %3d %3d %3d %3d %3d %3d %3d %3d\n' ...
         'DIFF:     %3d %3d %3d %3d %3d %3d %3d %3d\n\n'],...
        typecast(pi,'uint8'),tmp,abs(tmp-typecast(pi,'uint8')))

a.close
