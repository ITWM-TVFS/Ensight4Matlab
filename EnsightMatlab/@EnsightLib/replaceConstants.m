%% replaceConstants(object, constantList)
% Delete all constants and replace them with a set of new ones
%
% INPUT
%  object       : |EnsightLib| object
%  constantList : (Nx2 Cell-Array) {string, numeric}-tuples {'name',value}
%
% OUTPUT
%  none
%
% USAGE
%  object.replaceConstants(constantList)
%

%%
function replaceConstants(this, constantList)
    assert(this.inEditMode(),'EnsightObj::replaceConstants - Not in edit mode');
    assert(size(constantList,1)>0 && size(constantList,2) == 2, 'Wrong dimensions. 2xN cell array expected.');
  
    ok = 1;
  
    % Check input: (char x real) cell-array
    for i=1:size(constantList,1)
        ok = ok && isa(constantList{i,1},'char') && isnumeric(constantList{i,2});  
    end
    
    if ~ok    
        error('constantList is expected to be a Nx2 cell array with structure {(char)name,(double)value}');
    end
    
    valueArray = [2, 0, 5, 4];
    EnsightLib_interface('obj', 'replaceConstants', valueArray, this.getObjectHandle(), constantList);
    this.Constants = constantList;
end