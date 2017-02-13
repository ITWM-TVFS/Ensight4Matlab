%% getPartBounds(object, part, timestep)
% Compute geometric bounding box of the entire geometry
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
function Bbox = getPartBounds(this, part, timestep)
    assert(nargin<=3 && nargin>=2,'EnsightLib::getPartBounds - Invalid number of input arguments. Type `help EnsightLib.getPartBounds` for detailed information.');
    
    if nargin==2
        assert(~this.isTransient,'EnsightLib::getPartBounds - Object is not static. Type `help EnsightLib.getPartBounds` for detailed information.');
        timestep = 0;
    end
    
    pid = this.verifyPartIdentifier(part);
    assert(pid>-1,'EnsightLib::getPartBounds - Invalid part_id ');
    
    assert(this.verifyTimestep(timestep),'EnsightLib::getPartBounds - Invalid timestep. Use `EnsightLib.getTimeSteps` for a list of available timesteps.');
    
    valueArray = [3, 1, 5, 0, 0];
    buffer = EnsightLib_interface('part', 'getGeometryBounds', valueArray, this.getObjectHandle(), pid, timestep);
    Bbox = buffer;
end