%% createSubdivTree(object, maxDepth, maxElements, sizeOffset)
% Creates new |EnsightSubdivTree| and inserts all existing |EnsightParts|
%
% INPUT
%  object       : |EnsightObj| object
%  maxDepth     : (integer) maximum number of levels
%  maxElements  : (integer) maximum number of elements in a node before it
%                 gets split (unless maxDepth is reached)
%  sizeOffset   : (double) bounding box offset (optional, default: 0.0)
%
% OUTPUT
%  none
%
% USAGE
%  object.createSubdivTree(maxDepth, maxElements, (sizeOffset))
%

%%
function createSubdivTree(this, maxDepth, maxElements, sizeOffset)
  assert(~this.isTransient(),'EnsightLib::createSubdivTree - Transient data is not supported yet.');
  assert(~this.EnsightSubdivTree.created,'EnsightLib::createSubdivTree - There already exists a subdivision tree of this EnsightObject.');
  if nargin<4
    sizeOffset = 0;
  end

  valueArray = [4, 0, 5, 0, 0, 0];
  EnsightLib_interface('obj', 'createSubdivTree', valueArray, this.getObjectHandle(), maxDepth, maxElements, sizeOffset);
  this.EnsightSubdivTree = struct('created',true,'maxLevel',maxDepth,'maxElements',maxElements,'offset',sizeOffset,'bounds',[0 0; 0 0; 0 0]);
  
  valueArray = [1, 1, 5];
  this.EnsightSubdivTree.bounds = EnsightLib_interface('obj', 'getSubdivTreeBounds', valueArray, this.getObjectHandle());
end