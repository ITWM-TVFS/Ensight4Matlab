% getNumberOfParts(object)
% Returns the number of parts 
%
% INPUT
%  object   : |EnsightLib| object
%
% OUTPUT
%  num      : (int) 
%
% USAGE
%  num  = object.getNumberOfParts()
%

%%

function num = getNumberOfParts(this)
  num = size(this.EnsightPartList,1);
end