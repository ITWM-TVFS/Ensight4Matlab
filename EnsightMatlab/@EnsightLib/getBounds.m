%% getBounds(object, cell_list)
% Compute geometric boundingbox of the entire geometry
%
% INPUT
%  object   : |EnsightLib| object
%  timestep : (int) for transient objects only
%
% OUTPUT
%  bounds   : (numeric, vector, 3x2) axis-aligned, rectangular bounding box
%
% USAGE
%  bounds = object.getBounds(timesep) 
%  bounds = object.getBounds() (static object)
%

%%
function Bbox = getBounds(this, timestep)
    assert(nargin<=2 && nargin>=1,'EnsightLib::getBounds - Invalid number of input arguments. Type `help EnsightLib.getBounds` for detailed information.');
    
    if nargin==1
        assert(~this.isTransient,'EnsightLib::getBounds - Object is not static. Type `help EnsightLib.getBounds` for detailed information.');
        timestep = 0;
    end
    
    assert(this.verifyTimestep(timestep),'EnsightLib::getBounds - Invalid timestep. Use `EnsightLib.getTimeSteps` for a list of available timesteps.');
    
    valueArray = [2, 1, 5, 0];
    buffer = EnsightLib_interface('obj', 'getGeometryBounds', valueArray, this.getObjectHandle(), timestep);
    Bbox = buffer;
end