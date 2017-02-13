%% print(object)
% Displays detailed information about this object.
%
% INPUT
%  object : |EnsightObj| object
%
% OUTPUT
%  none
%
% USAGE
%  object.print
%

%%
function print(this)
  disp('-=-=-=-= EnsightObject =-=-=-=-');
  disp(' ');
  
  disp('== Subdivision tree linked to this object ==');
  if(this.EnsightSubdivTree.created)
      disp(['maxLevel    : ',num2str(this.EnsightSubdivTree.maxLevel)]);
      disp(['maxElements : ',num2str(this.EnsightSubdivTree.maxElements)]);
      disp(['offset      : ',num2str(this.EnsightSubdivTree.offset)]);
  else
      disp('Not created yet.');
  end
  disp(' ');
  
  disp('== TIMESTEPS ==');
  disp(this.timeSteps);
  fprintf('\n\n');

  fprintf('\t CONSTANTS (NAME, VALUE)\n');
  disp('------------------------------------------------');
  maxlength = 0;
  for i=1:size(this.Constants,1)
    maxlength = max(length(this.Constants{i,1}),maxlength);
  end
  for i=1:size(this.Constants,1)
    fprintf(['%',num2str(maxlength+1),'s \t %6.4f \n'],this.Constants{i,1},this.Constants{i,2});
  end
  fprintf('\n\n');

  fprintf('\t VARIABLES (NAME, TYPE, DIMENSIONS)\n');
  disp('------------------------------------------------');
  maxlength = 0;
  for i=1:size(this.EnsightVariableList,1)
    maxlength = max(length(this.EnsightVariableList{i,1}),maxlength);
  end
  for i=1:size(this.EnsightVariableList,1)
      d = this.EnsightVariableList{i,3};  
      fprintf(['%',num2str(maxlength+1),'s \t %15s \t%1d \n'],this.EnsightVariableList{i,1},this.EnsightVariableList{i,2},d(1));
  end
  fprintf('\n\n');
  
  fprintf('\t PARTS (ID, NAME) \n');
    disp('------------------------------------------------');
  for i=1:size(this.EnsightPartList,1)
    fprintf('%4d \t \t %s \n',this.EnsightPartList{i,1},this.EnsightPartList{i,2});
  end
  fprintf('\n\n');
  disp('================================================');
end