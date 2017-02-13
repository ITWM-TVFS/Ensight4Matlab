%% gerNumberOfVariables(object)
% Creates a new constant 'name' 
%
% INPUT
%  object : |EnsightLib| object
%
% OUTPUT
%  num    : (integer) number of variables related to this object
%
% USAGE
%  object.getNumberOfVariables
%  getNumberOfVariables(object)
%

function num = getNumberOfVariables(this)
  num = size(this.EnsightVariableList,1);
end