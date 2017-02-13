%% beginEdit(object)
% Enables edit mode 
%
% INPUT
%  object : |EnsightLib| object
%
% OUTPUT
%  none
%
% USAGE
%  object.beginEdit
%  beginEdit(object)
%

%%
function beginEdit(this)

    valueArray = [1, 0, 5];
    EnsightLib_interface('obj', 'beginEdit', valueArray, this.getObjectHandle());
    this.editMode = true; 
end