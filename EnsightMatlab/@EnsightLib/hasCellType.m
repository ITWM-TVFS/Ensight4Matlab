%% hasCellType(object, celltype, part, timestep)
% Checks existence of cells of a certain type, given an |EnsightPart| and a timestep
%
% INPUT
%  object   : |EnsightLib| object
%  part     : (int||string) part_identifier (name or id)
%  celltype : (int) 0-7 or (string) typename 
%  timestep : (integer) Not needed if the object is static
%
% OUTPUT
%  ok       : (bool) (true) if cells exists, (false) else
%  mOk      : (bool, nParts x nTimesteps) checks celltype for all parts/timesteps

% USAGE
%  ok = object.hasCellType(celltype, part, timestep)
%  mOk = object.hasCellType(celltype) 
%

%%
function ok = hasCellType(this, celltype, part, timestep)

    assert(nargin == 2 || nargin == 4, 'EnsightLib::hasCellType - Invalid number of input arguments. Type `help EnsightLib.hasCellType` for detailed information');
    valueArray = [4, 1, 5, 0, 0, 0];
  % Check input 
    if nargin == 4 % one timestep, one part
        celltype = this.verifyCellType(celltype);
        assert(celltype>=0,'EnsightLib::hasCellType - Input2(celltype) is not valid. Type `help EnsighLib.hasCellType` for detailed information');
        part = this.verifyPartIdentifier(part);
        assert(part>=0,'EnsightLib::hasCellType - Input3(part) is not valid. Type `help EnsighLib.hasCellType` for detailed information');
        assert(this.verifyTimestep(timestep),'EnsightLib::hasCellType - Input4(timestep) is not valid. Type `help EnsighLib.hasCellType` for detailed information');
        
        buffer = EnsightLib_interface('part', 'hasCellType', valueArray, this.getObjectHandle(), part, celltype, timestep);
        ok = buffer;x
    elseif nargin == 2
        celltype = this.verifyCellType(celltype);
        assert(celltype>=0,'EnsightLib::hasCellType - Input2(celltype) is not valid. Type `help EnsighLib.hasCellType` for detailed information');
        
        nParts = this.getNumberOfParts();
        nTimesteps = this.getNumberOfTimesteps();
        
        ok = zeros(nParts,nTimesteps);
        
        for p = 1:nParts
            for t = 1:nTimesteps
                buffer = EnsightLib_interface('part', 'hasCellType', valueArray, this.getObjectHandle(), this.EnsightPartList{p,1}, celltype, t-1);
                ok(p,t) = buffer;
            end
        end
    end
    
end