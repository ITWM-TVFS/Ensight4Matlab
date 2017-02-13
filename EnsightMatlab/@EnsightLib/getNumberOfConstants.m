%% getNumberOfConstants(object)
% Returns the number of constants 
%
% INPUT
%  object   : |EnsightLib| object
%
% OUTPUT
%  nC       : (int) 
%
% USAGE
%  nC   = object.getNumberOfConstants()
%

function num = getNumberOfConstants(this)
  num = size(this.Constants,1);
end