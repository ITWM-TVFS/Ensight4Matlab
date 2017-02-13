%% getConstants(object)
% Returns all constants assigned to this object 
%
% INPUT
%  object    : |EnsightLib| object
%
% OUTPUT
%  constants : (Nx2 Cell-Array) List of tuples {'name',value}
%
% USAGE
%  constants = object.getConstants
%

%%
function constants = getConstants(this)
  constants = this.Constants;
end