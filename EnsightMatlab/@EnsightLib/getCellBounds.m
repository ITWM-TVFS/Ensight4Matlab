%% getCellBounds(object, partID, cell, timestep)
% Compute geometric boundingbox of a single cell at a given timestep
%
% INPUT
%  object   : |EnsightLib| object
%  partID   : |String| name of part, or |int| id
%  cell     : (int, 1xM) cell nodes
% OUTPUT
%  bounds   : (numeric, vector, 3x2) axis-aligned, rectangular bounding box
%
% USAGE
%  bounds = object.getCellBounds(partID, cell) (static object)
%  bounds = object.getCellBounds(partID, cell, timestep) 
%

%%
function Bbox = getCellBounds(this, partID, cell, timestep)
    assert(nargin<=4 && nargin>=3,'EnsightLib::getCellBounds - Invalid number of input arguments. Type `help EnsightLib.getCellBounds` for detailed information.');
    
    if nargin==2
        assert(~this.isTransient,'EnsightLib::getCellBounds - Object is not static. Type `help EnsightLib.getCellBounds` for detailed information.');
        timestep = 0;
    end
    assert(this.verifyTimestep(timestep),'EnsightLib::getCellBounds - Invalid timestep. Use `EnsightLib.getTimeSteps` for a list of available timesteps.');
    
    partID = this.verifyPartIdentifier( partID );
    valueArray = [4, 1, 5, 0, 0, 0];
    buffer = EnsightLib_interface('part', 'getCellBounds', valueArray, this.getObjectHandle(), partID, cell, timestep);
    Bbox = buffer;
end