%% writeCase(object, filename, timestep)
% Enables editmode 
%
% INPUT
%  object   : |EnsightObj| object
%  filename : (string) name of the .encas file 
%  timestep : (integer) Not needed if the object is static
%
% OUTPUT
%  none
%
% USAGE
%  object.writeCase(filename)
%  object.writeCase(filename, timestep)
%

%%
function writeCase(varargin)
    this = varargin{1};
    assert(~this.inEditMode(),'EnsightLib::writeCase - Please leave edit mode before writing files.');
    filename = varargin{2};

    if nargin<3
        assert(~this.isTransient(),'EnsightLib::writeCase - Object is not static.');
        timestep = -1;
    else
        timestep = varargin{3};
    end
    
    % assert(this.verifyTimestep(timestep),'EnsightLib::writeCase - Invalid timestep.');
    
    valueArray = [4, 0, 5, 1, 0, 0];
    EnsightLib_interface('obj', 'write', valueArray, this.getObjectHandle(), filename, 0, timestep);
end