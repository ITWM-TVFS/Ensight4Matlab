%% inEditMode(object)
% Checks, whether this object is in edit mode or not.
% Note: Most operations may only performed, while an object is in edit mode!
%
% INPUT
%  object : |EnsightObj| object
%
% OUTPUT
%  mode   : (bool) editmode 
%
% USAGE
%  mode = object.inEditMode
%

%%
function ok = inEditMode(this)
  ok = this.editMode;
end