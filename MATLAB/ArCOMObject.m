%{
----------------------------------------------------------------------------

This file is part of the Sanworks ArCOM repository
Copyright (C) 2016 Sanworks LLC, Sound Beach, New York, USA

----------------------------------------------------------------------------

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3.

This program is distributed  WITHOUT ANY WARRANTY and without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
%}

% ArCOM uses an Arduino library to simplify serial communication of different
% data types between MATLAB/GNU Octave and Arduino. To use the library,
% include the following at the top of your Arduino sketch:
% #include "ArCOM.h". See documentation for more Arduino-side tips.
%
% Initialization syntax:
% MyPort = ArCOMObject('COM3', 115200)
% where 'COM3' is the name of Arduino's serial port on your system, and 115200 is the baud rate.
% This call both creates and opens the port. It returns an object containing
% a serial port and properties. If PsychToolbox IOport interface is
% available, this is used by default. To use the java interface on a system
% with PsychToolbox, use ArCOM('open', 'COM3', 'java')
%
% Write: MyPort.write(myData, 'uint8') % where 'uint8' is a
% data type from the following list: 'uint8', 'uint16', 'uint32', 'int8', 'int16', 'int32', 'char'.
% If no data type argument is specified, ArCOM assumes uint8. Additional
% pairs of vectors and types can be added, to be packaged into a single
% write operation i.e. MyPort.write(Data1, Type1, Data2, Type2,... DataN, TypeN)
%
% Read: myData = MyPort.read(nValues, 'uint8') % where nValues is the number
% of values to read, and 'uint8' is a data type (see Write above for list)
% If no data type argument is specified, ArCOM assumes uint8. Additional
% pairs of value numbers and types can be added, to be packaged into a single
% read operation i.e. [Array1, Array2] = MyPort.write(Data1, Type1, Data2, Type2)
%
% End: MyPort.close() % Closes, deletes and clears the serial port
% object in the workspace of the calling function. You can also type clear
% MyPort - the object destructor will automatically close the port.

classdef ArCOMObject < handle
    properties
        baudRate                % Baud rate for serial transmission
    end

    properties (Dependent)
        bytesAvailable          % Number of bytes available for reading
    end

    properties (Hidden)
        Port = []               % Port object
    end

    properties (Dependent, Hidden)
        BytesAvailableFcn       % Bytes available callback function
    end

    properties (Access = private)
        isLittleEndian          % Are we using little-endian byte ordering?
        isOctave                % Are we using GNU/Octave?
        hasPsychToolbox         % Is PsychToolbox available?
        Interface = 0
        validDataTypes = ...    % List of supported data-types
            {'int8','uint8','char','int16','uint16',...
            'int32','uint32','int64','uint64','logical','single','double'};
        BytesAvailableFcnCount = 64 % Number of bytes of data to trigger callback
        inputBuffer = 1E5;      % Size of input buffer
        outputBuffer = 1E5;     % Size of output buffer
        timeout = 1;            % Serial timeout in seconds
        writeFcn                % Anonymous function: write data
        readFcn                 % Anonymous function: read data
        flushFcn                % Anonymous function: flush serial port
        getBytesAvailableFcn    % Anonymous function: get available bytes
    end

    methods
        function obj = ArCOMObject(portString, baudRate, forceOption, varargin)

            % Obtain information on the current system
            [~,~,tmp] = computer;
            obj.isLittleEndian = strcmp(tmp,'L');
            obj.isOctave = exist('OCTAVE_VERSION', 'builtin') > 0;
            obj.hasPsychToolbox = exist('PsychtoolboxVersion', 'file') > 0;

            % Prepare things for GNU/Octave
            if obj.isOctave
                % Load Instrument Control Toolbox
                tmp = ver('instrument-control');
                assert(~isempty(tmp),['Please install the instrument ' ...
                    'control toolbox first. See ' ...
                    'http://wiki.octave.org/Instrument_control_package']);
                pkg load instrument-control

                % Disable warning for implicit conversions of numbers to
                % their UTF-8 encoded character equivalents when strings
                % are constructed using a mixture of strings and numbers in
                % matrix notation.
                warning('off', 'Octave:num-to-str');
                obj.UseOctave = 1;
                obj.Interface = 2; % Octave serial interface
            else
                obj.UseOctave = 0;
            end

            % Try to guess portString / validate argument
            if ~exist('portString','var') || isempty(portString)
                if exist('serialportlist','file')
                    tmp = sort(serialportlist('available'));
                elseif exist('seriallist','file')
                    if obj.isOctave
                        if isunix
                            tmp = strcat('/dev/',seriallist);
                        else
                            tmp = seriallist;
                        end
                    else
                        tmp = sort(seriallist('available'));
                    end
                else
                    tmp = [];
                end
            else
                error('Error: Please add a baudRate argument when creating an ArCOM object')
            end
            if isunix
                assert(exist(portString,'file')>0,...
                    '''%s'' is not a valid serial port',portString)
            end

            % Validate baudRate argument
            if ~exist('baudRate','var') || isempty(baudRate)
                obj.baudRate = 9600;
            else
                validateattributes(baudRate,{'numeric'},...
                    {'scalar','integer','>=',110},'','BAUDRATE')
                obj.baudRate = baudRate;
            end

            % Validate manual choice of interface
            if exist('forceOption','var') && ~isempty(forceOption)
                forceOption = validatestring(lower(forceOption),...
                    {'java','psychtoolbox'},'','FORCEOPTION');
            else
                error(['Error: ' baudRate ' is an invalid baud rate for ArCOM. Some common baud rates are: 9600, 115200'])
            end

            % Pick serial interface
            if obj.hasPsychToolbox && ~strcmp(forceOption,'java')
                obj.Interface = 1;      % PsychToolbox interface
            elseif obj.isOctave
                tmp = instrhwinfo().SupportedInterfaces;
                if any(~cellfun(@isempty,strfind(tmp,'serialport'))) %#ok<STRCLFH>
                    obj.Interface = 3;  % Octave serialport interface
                elseif any(~cellfun(@isempty,strfind(tmp,'serial'))) %#ok<STRCLFH>
                    obj.Interface = 2;  % Octave serial interface
                else
                    error('Serial communication is not supported on your platform.');
                end
            else
                if verLessThan('matlab','9.7')
                    obj.Interface = 0;  % MATLAB serial interface
                else
                    obj.Interface = 3;  % MATLAB serialport interface
                end
            end

            % Validate additional options
            if mod(numel(varargin),2)
                error('Incorrect number of arguments')
            end
            if numel(varargin)>1
                argnames = varargin(1:2:end);
                argvals  = varargin(2:2:end);
                for ii = 1:numel(argnames)
                    validateattributes(argnames{ii},{'string','char'},{'row'})
                    switch(lower(argnames{ii}))
                        case 'outputbuffersize'
                            obj.outputBuffer = argvals{ii};
                        case 'inputbuffersize'
                            obj.inputBuffer = argvals{ii};
                        case 'timeout'
                            obj.timeout = argvals{ii};
                        case 'bytesavailablefcn'
                            obj.BytesAvailableFcn = argvals{ii};
                        case 'bytesavailablefcncount'
                            obj.BytesAvailableFcnCount = argvals{ii};
                        otherwise
                            error('Unknown parameter: ''%s''.',argnames{ii})
                    end
                end
            end

            % Initialize serial interface
            switch obj.Interface
                case 0  % MATLAB (deprecated serial interface)
                    obj.Port = serial(portString, ...
                        'BaudRate',                 obj.baudRate, ...
                        'Timeout',                  obj.timeout, ...
                        'OutputBufferSize',         obj.outputBuffer, ...
                        'InputBufferSize',          obj.inputBuffer, ...
                        'BytesAvailableFcn',        obj.BytesAvailableFcn, ...
                        'BytesAvailableFcnCount',   obj.BytesAvailableFcnCount, ...
                        'BytesAvailableFcnMode',    'Byte', ...
                        'DataTerminalReady',        'on', ...
                        'Tag',                      'ArCOM'); %#ok<*SERIAL>
                    fopen(obj.Port);
                case 1
                    if ispc
                        portString = ['\\.\' portString];
                    end
                    IOPort('Verbosity', 0);
                    obj.Port = IOPort('OpenSerialPort', portString, 'BaudRate=115200, OutputBufferSize=100000, InputBufferSize=100000, DTR=1');
                    if (obj.Port < 0)
                        try
                            IOPort('Close', obj.Port);
                        catch
                        end
                        error(['Error: Unable to connect to port ' portString '. The port may be in use by another application.'])
                    end
                    pause(.1); % Helps on some platforms
                    varargout{1} = obj;
                case 2
                    if ispc
                        PortNum = str2double(portString(4:end));
                        if PortNum > 9
                            portString = ['\\\\.\\COM' num2str(PortNum)]; % As of Octave instrument control toolbox v0.2.2, ports higher than COM9 must use this syntax
                        end
                    end
                    obj.Port = serial(portString, obj.baudRate, obj.timeout);
                    obj.writeFcn = @(data) srl_write(obj.Port,char(data));
                    obj.readFcn = @(nBytes) srl_read(obj.Port,nBytes);
                    obj.flushFcn = @() srl_flush(obj.Port);
                    obj.getBytesAvailableFcn = @() get(obj.Port,'bytesavailable');

                case 3  % MATLAB & Octave (serialport interface)
                    obj.Port = serialport(portString, obj.baudRate);
                    obj.Port.Timeout = obj.timeout;
                    obj.writeFcn = @(data) write(obj.Port,data,'uint8');
                    obj.readFcn = @(nBytes) read(obj.Port,nBytes,'uint8');
                    obj.flushFcn = @() flush(obj.Port);
                    obj.getBytesAvailableFcn = @() obj.Port.NumBytesAvailable;
                    if ~obj.isOctave
                        configureCallback(obj.Port,'Byte',...
                            obj.BytesAvailableFcnCount,obj.BytesAvailableFcn)
                    end
            end
            pause(.2);
            obj.flush;
        end

        function out = get.bytesAvailable(obj)
            out = obj.getBytesAvailableFcn();
        end

        function out = get.BytesAvailableFcn(obj)
            assert(~obj.isOctave,   'BytesAvailableFcn is not available with GNU/Octave.')
            assert(obj.Interface~=1,'BytesAvailableFcn is not available with PsychToolbox.')
            out = obj.Port.BytesAvailableFcn;
        end

        function set.BytesAvailableFcn(obj,in)
            assert(~obj.isOctave,   'BytesAvailableFcn is not available with GNU/Octave.')
            assert(obj.Interface~=1,'BytesAvailableFcn is not available with PsychToolbox.')
            switch obj.Interface
                case 0
                    obj.Port.BytesAvailableFcn = in;
                case 3
                    configureCallback(obj.Port,"byte",obj.BytesAvailableFcnCount,in)
            end
        end

        function flush(obj)
            obj.flushFcn();
        end
        function write(obj, varargin)
            if nargin == 2 % Single array with no data type specified (defaults to uint8)
                nArrays = 1;
                data2Send = varargin(1);
                dataTypes = {'uint8'};
            else
                nArrays = (nargin-1)/2;
                data2Send = varargin(1:2:end);
                dataTypes = varargin(2:2:end);
            end

            % Ensure data is sent little-endian
            if ~obj.isLittleEndian
                data = cellfun(@(x) {swapbytes(x)},data);
            end

            % Cast data to uint8 & write to serial
            for ii = 1:numel(data)
                type = types{ii};
                if ~isempty(strfind(type,'int')) %#ok<STREMP>
                    data{ii} = OORcast(data{ii},type,intmin(type),intmax(type));
                    data{ii} = typecast(data{ii},'uint8');
                elseif ismember(type,'char')
                    data{ii} = OORcast(data{ii},type,0,128);
                elseif ismember(type,{'single','double'})
                    data{ii} = cast(data{ii},type);
                    data{ii} = typecast(data{ii},'uint8');
                elseif ismember(type,'logical')
                    data{ii} = uint8(logical(data{ii}));
                end
            end
            obj.writeFcn([data{:}]);

            % Local function for checking range & casting
            function out = OORcast(in,type,typeMin,typeMax)
                isOOR = in<typeMin | in>typeMax;
                assert(all(~isOOR),['Value %d is out of range for data '...
                    'type ''%s''. Valid range: %d to %d.'], ...
                    int64(in(find(isOOR,1))),type,typeMin,typeMax)
                out = cast(in,type);
            end
        end
        function varargout = read(obj, varargin)
            if nargin == 2
                nArrays = 1;
                nValues = varargin(2);
                dataTypes = {'uint8'};
            else
                nArrays = (nargin-1)/2;
                nValues = varargin(1:2:end);
                dataTypes = varargin(2:2:end);
            end
            nArrays = numel(nValues);

            % Calculate total number of bytes to read
            nBytes = nValues;
            tmp = ismember(types,{'int16','uint16'});
            nBytes(tmp) = nBytes(tmp) * 2;
            tmp = ismember(types,{'int32','uint32','single'});
            nBytes(tmp) = nBytes(tmp) * 4;
            tmp = ismember(types,{'int64','uint64','double'});
            nBytes(tmp) = nBytes(tmp) * 8;
            nBytesTotal = sum(nBytes);

            % Read serial data
            data = uint8(obj.readFcn(nBytesTotal));
            assert(~isempty(data),'The serial port returned 0 bytes.')

            % Ensure data is returned as a row vector
            data = reshape(data,1,[]);

            % Split data & return cast
            varargout = cell(1,nArrays);
            for i = 1:nArrays
                switch dataTypes{i}
                    case 'char'
                        varargout{i} = char(ByteString(Pos:Pos+nValues(i)-1)); Pos = Pos + nValues(i);
                    case 'uint8'
                        varargout{i} = uint8(ByteString(Pos:Pos+nValues(i)-1)); Pos = Pos + nValues(i);
                    case 'uint16'
                        varargout{i} = typecast(uint8(ByteString(Pos:Pos+(nValues(i)*2)-1)), 'uint16'); Pos = Pos + nValues(i)*2;
                    case 'uint32'
                        varargout{i} = typecast(uint8(ByteString(Pos:Pos+(nValues(i)*4)-1)), 'uint32'); Pos = Pos + nValues(i)*4;
                    case 'int8'
                        varargout{i} = typecast(uint8(ByteString(Pos:Pos+(nValues(i))-1)), 'int8'); Pos = Pos + nValues(i);
                    case 'int16'
                        varargout{i} = typecast(uint8(ByteString(Pos:Pos+(nValues(i)*2)-1)), 'int16'); Pos = Pos + nValues(i)*2;
                    case 'int32'
                        varargout{i} = typecast(uint8(ByteString(Pos:Pos+(nValues(i)*4)-1)), 'int32'); Pos = Pos + nValues(i)*4;
                end
            end

            % Ensure endianness
            if ~obj.isLittleEndian
                varargout = cellfun(@(x) {swapbytes(x)},varargout);
            end
        end
        function delete(obj)
            switch obj.Interface
                case 0
                    if ~isempty(obj.Port) && isvalid(obj.Port)
                        fclose(obj.Port);
                    end
                case 1
                    IOPort('Close', obj.Port);
                case 2
                    if ~isempty(obj.Port)
                        obj.Port.close;
                    end
            end
        end
        function close(obj)
            if obj.isOctave
                obj.delete;
            end
            evalin('caller', ['clear ' inputname(1)])
        end
    end

    methods (Access = private)
        function flushSerial(obj)
            % flush both in- and outputs with deprecated serial interface
            flushinput(obj.Port);
            flushoutput(obj.Port);
        end

        function out = parseTypes(obj,in)
            out = lower(in);
            valid = ismember(out,obj.validDataTypes);
            assert(all(valid),['Data type ''%s'' is not currently ' ...
                'supported by ArCOM.'],out{find(~valid,1)})
        end
    end
end
