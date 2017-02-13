%% getMaxId(object)
% Returns highest part_id in use
%
% INPUT
%  object : |EnsightLib| object
%
% OUTPUT
%  id     : (integer)
%
% USAGE
%  object.getMaxId
%  getMaxId(object)
%

%%
function id = getMaxId(this)
  id = max(cell2mat(this.EnsightPartList(:,1)));
  if isempty(id)
      id = 0;
  end
end