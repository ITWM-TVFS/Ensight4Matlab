%% endEdit(object)
% Disables editmode 
%
% INPUT
%  object : |EnsightLib| object
%
% OUTPUT
%  none
%
% USAGE
%  object.endEdit
%

%%
function endEdit(this)
   
    valueArray = [1, 0, 5];
    EnsightLib_interface('obj', 'endEdit', valueArray, this.getObjectHandle());
    this.editMode = 0;
end