%% getCellValues(object, partID, celltypeID, timestep)
% Returns nodes of all cells, given an |EnsightPart| and a timestep
%
% INPUT
%  object   : |EnsightLib| object
%  partID   : |String| name of part, or |int| id
%  timestep : 
%
% OUTPUT
%  values   : (MxN) matrix
%              M    depends on celltype  
%              N    number of cells
%             
% USAGE
%  values = object.getCellValues(partID, celltypeID, timestep)
%  values = object.getCellValues(partID, celltypeID) (static)
%

%%
function values = getCellValues(this, partID, celltypeID, timestep)
    
  assert(nargin >= 3 && nargin <= 4, 'EnsightLib::getCellValues - Invalid number of input arguments. Type `help EnsightLib.getCellValues` for detailed information');
    
  % Check input 
    if nargin == 4 % timestep given
        assert(isnumeric(timestep),'EnsightLib::getCellValues - Input(timestep) is not valid. Type `help EnsighLib.getCellValues` for detailed information');
    elseif nargin == 3 
        assert(~this.isTransient(),'EnsightLib::getCellValues - Object is not static. Timestep must be specified. Type `help EnsighLib.getCellValues` for detailed information');
            timestep = 0;
    end
    
    celltypeID = this.verifyCellType( celltypeID );
    partID = this.verifyPartIdentifier( partID );
    
    assert(timestep <= length(this.getTimesteps),'EnsightLib::getCellValues - Invalid timestep. Type `object.getTimesteps` to see all existing timesteps or `help EnsightLib.getCellValues` for further information');
    
    valueArray = [4, 1, 5, 0, 0, 0];
    buffer = EnsightLib_interface('part', 'getCellValues', valueArray, this.getObjectHandle(), partID, celltypeID, timestep);
    values = buffer;
end