%% MATLAB wrapper for EnsightLib
% SETUP
% Specifies the path settings of all external libraries.
%
function SETUP()
    if exist('SETTINGS.cfg','file')==2
        load('SETTINGS.cfg',...
             'QT_INCLUDE_PATH',...
             'QT_LIB_PATH',...
             'EIGEN_PATH',...
             'ENSIGHT_INCLUDE_PATH',...
             'ENSIGHT_LIB_PATH','-mat');

         fprintf('QT_INCLUDE_PATH: \n \t %s \n',QT_INCLUDE_PATH);
         ok = input('Keep old path? (y/n) \n >> ','s');
         while all(ok=='n') || all(ok=='N')
             new = input('New QT_INCLUDE_PATH (containing the "QtCore" directory, e.g. "/usr/include"):\n >> ','s');
             if(exist(new,'dir')~= 7 || exist([new,'/QtCore/qstring.h'],'file')~= 2);
                 warning('The new path seems to be incorrect. ');
                 ok = input('Keep old path? (y/n) \n >> ','s');
             else
                 QT_INCLUDE_PATH = new;
                 ok = 'y';
             end
         end

         fprintf('QT_LIB_PATH: \n \t %s \n', QT_LIB_PATH);
         ok = input('Keep old path? (y/n) \n >> ','s');
         while all(ok=='n') || all(ok=='N')
             new = input('New QT_LIB_PATH (containing library "libQtCore.so", e.g. "/usr/lib64"):\n >> ','s');
             if(exist(new,'dir')~= 7 || exist([new,'/libQtCore.so'],'file')~= 2);
                 warning('The new path seems to be incorrect. ');
                 ok = input('Keep old path? (y/n) \n >> ','s');
             else
                 QT_LIB_PATH = new;
                 ok = 'y';
             end
         end

         fprintf('EIGEN_PATH: \n \t %s \n',EIGEN_PATH);
         ok = input('Keep old path? (y/n) \n >> ','s');
         while all(ok=='n') || all(ok=='N')
             new = input('New EIGEN_PATH:\n >> ','s');
             if(exist(new,'dir')~= 7 || exist([new,'/Eigen/Dense'],'file')~= 2)
                 warning('The new path seems to be incorrect. ');
                 ok = input('Keep old path? (y/n) \n >> ','s');
             else
                 EIGEN_PATH = new;
                 ok = 'y';
             end
         end

         fprintf('ENSIGHT_INCLUDE_PATH: \n \t %s \n',ENSIGHT_INCLUDE_PATH);
         ok = input('Keep old path? (y/n) \n >> ','s');
         while all(ok=='n') || all(ok=='N')
             new = input('New ENSIGHT_INCLUDE_PATH:\n >> ','s');
             if(exist(new,'dir')~= 7 || exist([new,'/ensightlib.h'],'file')~= 2)
                 warning('The new path seems to be incorrect. ');
                 ok = input('Keep old path? (y/n) \n >> ','s');
             else
                 ENSIGHT_INCLUDE_PATH = new;
                 ok = 'y';
             end
         end

         fprintf('ENSIGHT_LIB_PATH: \n \t %s \n',ENSIGHT_LIB_PATH);
         ok = input('Keep old path? (y/n) \n >> ','s');
         while all(ok=='n') || all(ok=='N')
             new = input('New ENSIGHT_INCLUDE_PATH:\n >> ','s');
             if(exist(new,'dir')~= 7)
                 warning('The new path seems to be incorrect. ');
                 ok = input('Keep old path? (y/n) \n >> ','s');
             else
                 ENSIGHT_LIB_PATH = new;
                 ok = 'y';
             end
         end

        save('SETTINGS.cfg', 'QT_INCLUDE_PATH', 'QT_LIB_PATH', 'EIGEN_PATH', ...
             'ENSIGHT_INCLUDE_PATH', 'ENSIGHT_LIB_PATH', '-mat');
         disp('Setings have been saved.');
         disp('Call "runmex" in order to compile EnsightLib');
    else
        disp('"SETTINGS.cfg" has not been found.');
        disp('The absolute paths to "Qt 4", "Eigen" (3.1.1 or greater), and "EnsightLib"');
        disp('are needed to compile the MEX files.');
        disp('Enter the corresponding paths below:');

        ok = false;
        while ok==false
             new = input('QT_INCLUDE_PATH (containing the "QtCore" directory, e.g. "/usr/include"):\n >> ','s');
             if(exist(new,'dir')~= 7 || exist([new,'/QtCore/qstring.h'],'file')~= 2);
                 warning('The path seems to be incorrect. ');
             else
                 ok = true;
                 QT_INCLUDE_PATH = new;
             end
        end

        ok = false;
        while ok==false
             new = input('QT_LIB_PATH (containing library "libQtCore.so", e.g. "/usr/lib64"):\n >> ','s');
             if(exist(new,'dir')~= 7 || exist([new,'/libQtCore.so'],'file')~= 2);
                 warning('The path seems to be incorrect. ');
             else
                 ok = true;
                 QT_LIB_PATH = new;
             end
        end
         
        ok = false;
        while ok==false
             new = input('EIGEN_PATH:\n >> ','s');
             if(exist(new,'dir')~= 7 || exist([new,'/Eigen/Dense'],'file')~= 2)
                 warning('The path seems to be incorrect. ');
             else
                 ok = true;
                 EIGEN_PATH = new;
             end
        end
        
        ok = false;
        while ok==false
             new = input('ENSIGHT_INCLUDE_PATH:\n >> ','s');
             if(exist(new,'dir')~= 7 || exist([new,'/ensightlib.h'],'file')~= 2)
                 warning('The path seems to be incorrect. ');
             else
                 ok = true;
                 ENSIGHT_INCLUDE_PATH = new;
             end
        end
        
        ok = false;
        while ok==false
             new = input('ENSIGHT_LIB_PATH:\n >> ','s');
             if(exist(new,'dir')~= 7)
                 warning('The path seems to be incorrect. ');
             else
                 ok = true;
                 ENSIGHT_LIB_PATH = new;
             end
        end

        save('SETTINGS.cfg', 'QT_INCLUDE_PATH', 'QT_LIB_PATH', 'EIGEN_PATH', ...
             'ENSIGHT_INCLUDE_PATH', 'ENSIGHT_LIB_PATH', '-mat');
        disp('Setings have been saved.');
        disp('Call "runmex" in order to compile EnsightLib Matlab interface');
    end
end