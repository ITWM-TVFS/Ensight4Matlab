%% getVertices(object, part, timestep)
% Returns the vertices of an |EnsightPart| for multiple timesteps
%
% INPUT
%  object   : |EnsightLib| object
%  part     : (int || string) EnsightPart identifier
%  timestep : (integer) single timestep
%           : (vector) multiple timesteps
%
% OUTPUT
%  vertices : (double, 3xN matrix) 
%  v1,..,vn : (double, 3xN matrices) for n timesteps
%  V{1:n}   : (1xn cell) {3xN double} for n timesteps
%
% USAGE
%  V            = object.getVertices() (static object with one single EnsightPart)
%  V            = object.getVertices(part) (static object)
%  V            = object.getVertices(timestep) (only one EnsightPart exists)
%  [V1,..,Vn]   = object.getVertices([t1,..,tn]) (only one EnsightPart exists)
%  V{}          = object.getVertices([t1,..,tn]) (only one EnsightPart exists)
%  V            = object.getVertices(part, timestep)
%  [V1,..,Vn]   = object.getVertices(part, [t1,..,tn])
%  V{}          = object.getVertices(part, [t1,..,tn])

%%
function varargout = getVertices(varargin)
    assert(nargin >= 1 && nargin <= 3, 'EnsightLib::getVertices - Invalid number of input arguments. Type `help EnsightLib.getVertices` for detailed information');
    
    % Check input 
    if nargin == 3 % one timestep, one part
        this = varargin{1};
        part = this.verifyPartIdentifier(varargin{2});
        assert(part>=0,'EnsightLib::getVertices - Input2(part) is not valid. Type `help EnsighLib.getVertices` for detailed information');
        timestep = varargin{3};
        assert(isnumeric(timestep),'EnsightLib::getVertices - Input3(timestep) is not valid. Type `help EnsighLib.getVertices` for detailed information');
    elseif nargin == 2 
        this = varargin{1};
        assert(size(this.EnsightPartList,1)==1 || ~this.isTransient(),'EnsightLib::getVertices - Object is either transient or contains multiple parts. Input2 must be specified. Type `help EnsighLib.getVertices` for detailed information');
        if(this.isTransient()) % timestep given, only on part exists
            part = this.EnsightPartList{1,1};
            timestep = varargin{2};
            assert(isnumeric(timestep),'EnsightLib::getVertices - Input2(timestep) is not valid. Type `help EnsighLib.getVertices` for detailed information');
        else % static objet, part_id given
            if isnumeric(varargin{2})
                assert(length(varargin{2})==1,'EnsightLib::getVertices - Input2(part) is not valid. Type `help EnsighLib.getVertices` for detailed information');
            end
            part = this.verifyPartIdentifier(varargin{2});
            assert(part>=0,'EnsightLib::getVertices - Input2(part) is not valid. Type `help EnsighLib.getVertices` for detailed information');
            timestep = 0;
        end
    elseif nargin == 1
        this = varargin{1};
        assert(size(this.EnsightPartList,1)==1 && ~this.isTransient(),'EnsightLib::getVertices - Object is either transient or contains multiple parts. Input2 must be specified. Type `help EnsighLib.getVertices` for detailed information');
        timestep = 0;
        part = this.EnsightPartList{1,1};
    end
   
    nt = length(timestep);
    assert(all(timestep>=0) && all(timestep<=this.getNumberOfTimesteps) && all(floor(timestep)==timestep),'EnsightLib::getVertices - Invalid timestep(s). Type `object.getTimesteps` to see all existing timesteps or `help EnsightLib.getVertices` for further information');
    
    valueArray = [3, 1, 5, 0, 0];
    tvector = this.getTimesteps();
    if nt>1 % multiple timesteps at once
        assert(nargout==nt || nargout==1,'EnsightLib::getVertices - Number of output arguments does not match number of timesteps. Type `help EnsightLib.getVertices` for detailed information.');
        if nargout == 1
            V = cell(1,nt);
            for t=1:nt % cell output
                buffer = EnsightLib_interface('part', 'getVertices', valueArray, this.getObjectHandle(), part, tvector(t));
                V{t} = buffer;
            end
            varargout{1} = V;
        else % multiple output arguments
            varargout = cell(1, nt);
            for t=1:nt
                buffer = EnsightLib_interface('part', 'getVertices', valueArray, this.getObjectHandle(), part, tvector(t));
                varargout{t} = buffer;
            end
        end
    else % single timestep
        buffer = EnsightLib_interface('part', 'getVertices', valueArray, this.getObjectHandle(), part, timestep);
        varargour{1} = buffer;
    end
end

%% TODO: Heavy function overloading. Split into multiple sub-routines