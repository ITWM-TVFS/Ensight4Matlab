%% setVertices(object, part, vertices, timestep)
% Sets geometry vertices of this object
%
% INPUT
%  object   : |EnsightObj| object
%  part     : (int|string) part_idetifier
%  vertices : 3xN (double)Matrix representing a set of 3-dimensional points
%  timestep : integer timestep_id, multiple values possible
%
% OUTPUT
%  none
%
% USAGE
%  object.setVertices(part, vertices, timestep)
%  object.setVertices(part, vertices) (static objects only)
%

%%
function setVertices(this, part, vertices, timestep)
    % Check edit_mode
    assert(this.inEditMode(),'EnsightLib::setVertices - Not in edit mode');
    
    assert(size(vertices,1)==3,'EnsightLib::setVertices - Wrong matrix dimensions. 3xN expected.');
    
    if nargin<4
        assert(~this.isTransient(),'EnsightLib::setVertices - Object is not static. Please specify a timestep.');
        timestep = 0;
    else
        assert(this.verifyTimestep(timestep),'EnsightLib::setVertices - Invalid timestep vector.');
    end
            
    % 
    pid = this.verifyPartIdentifier(part);
    assert(pid>-1,'EnsightObj::setVertices - Invalid part identifier');
    
    valueArray = [4, 0, 5, 0, 0, 0];
    EnsightLib_interface('part', 'setVertices', valueArray, this.getObjectHandle(), pid, vertices, timestep);
end