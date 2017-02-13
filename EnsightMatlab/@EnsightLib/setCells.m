%% setCells(object, part, cells, celltype, timestep)
%  
%
% INPUT
%  object   : |EnsightLib| object
%  part     : (int|string) part_idetifier
%  cells    : (int,matrix) pointer to local coordinates
%  celltype : (int|string) celltype identifier
%  timestep : (int) timestep_index (transient objects only)
%
% OUTPUT
%  none
%
% USAGE
%  object.setCells(part, cells, celltype)
%  object.setCells(part, cells, celltype, timestep)
%

%%
function setCells(this, part, cells, celltype, timestep)

    if(nargin <5 && this.isTransient)
        error('EnsightLib::setCells - Object is not static. Timestep required.');
    else
        timestep = 0;
    end
    
    % Determine part_id
    pid = this.verifyPartIdentifier(part);
    assert(pid>0,'EnsightLib::setCells - Invalid part identifier');
    
    % Determine celltype_id
    celltype_id = this.verifyCellType(celltype);
    assert(celltype_id>-1,'EnsightLib::setCells - Invalid celltype identifier');

    % Verify correct dimensions of 
    assert(size(cells,1) == this.getNumberOfNodes(celltype_id),'EnsightLib::setCells - Invalid cell dimension');
    
    % Verify timestep
    assert(this.verifyTimestep(timestep),'EnsightLib::setCells - Invalid timestep');
    
    valueArray = [5, 0, 5, 0, 0, 0, 0];
    EnsightLib_interface('part', 'setCells', valueArray, this.getObjectHandle(), pid , cells, timestep, celltype_id);
end