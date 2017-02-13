%% MATLAB wrapper for EnsightLib
% runmex
% Compiles the binaries needed by all MATLAB classes contained in
% EnsightLib
%
% Make sure to run SETUP before you call this function for the first time.
%
function runmex()
    if exist('EnsightLib_interface.mexa64','file')==3 && mislocked('EnsightLib_interface')==1
        error('EnsightLib_interface is still locked. Use "munlock()" before re-compiling');
    end
    if exist('SETTINGS.cfg','file')==2
        load('SETTINGS.cfg',...
             'QT_INCLUDE_PATH',...
             'QT_LIB_PATH',...
             'EIGEN_PATH',...
             'ENSIGHT_INCLUDE_PATH',...
             'ENSIGHT_LIB_PATH','-mat');
         
        OPTS = '-DMEX_INCLUDE -cxx -compatibleArrayDims -v ';
        mex_str = ['mex ',OPTS,...
                   'CXXFLAGS="\$CXXFLAGS -std=c++11"', ...
                   ' -I',QT_INCLUDE_PATH,' -I',QT_INCLUDE_PATH,'/QtCore',...
                   ' -I',EIGEN_PATH,...
                   ' -I',ENSIGHT_INCLUDE_PATH,...
                   ' -L',QT_LIB_PATH,' -lQtCore -L',ENSIGHT_LIB_PATH,' -lEnsightLib '];

        fprintf('PATH settings\n');
        fprintf('  QT(header) \t\t\t:\t %s\n', QT_INCLUDE_PATH);
        fprintf('  QT(lib) \t\t\t:\t %s\n', QT_LIB_PATH);
        fprintf('  Eigen \t\t:\t %s\n', EIGEN_PATH);
        fprintf('  Ensight(header) \t:\t %s\n', ENSIGHT_INCLUDE_PATH);
        fprintf('  Ensight(lib) \t\t:\t %s\n', ENSIGHT_LIB_PATH);

        ok = 1;

        if(exist(QT_INCLUDE_PATH,'dir')~= 7 || exist([QT_INCLUDE_PATH,'/QtCore/qstring.h'],'file')~= 2)
            warning('QT_INCLUDE_PATH seems to be incorrect. Please run "SETUP".');
            ok = 0;
        end
        if(exist(QT_LIB_PATH,'dir')~= 7 || exist([QT_LIB_PATH,'/libQtCore.so'],'file')~= 2)
            warning('QT_LIB_PATH seems to be incorrect. Please run "SETUP".');
            ok = 0;
        end
        if(exist(EIGEN_PATH,'dir')~= 7 || exist([EIGEN_PATH,'/Eigen/Dense'],'file')~= 2)
            warning('EIGEN_PATH seems to be incorrect. Please run "SETUP".');
            ok = 0;
        end
        if(exist(ENSIGHT_INCLUDE_PATH,'dir')~= 7 || exist([ENSIGHT_INCLUDE_PATH,'/ensightlib.h'],'file')~= 2)
            warning('ENSIGHT_INCLUDE_PATH seems to be incorrect. Please run "SETUP".');
            ok = 0;
        end
        if(exist(ENSIGHT_LIB_PATH,'dir')~= 7)
            warning('ENSIGHT_LIB_PATH seems to be incorrect. Please run "SETUP".');
            ok = 0;
        end

        if ok
            fprintf('\nCompiling EnsightLib_interface ...\n');

            %% EnsightLib_interface
            eval([mex_str,' mexfiles/EnsightLib_interface.cpp']);

            fprintf('... done\n');
        else
            error('Could not compile EnsightLib_interface.');
        end
    else
        error('"SETTINGS.cfg" could not be found. Call "SETUP" in order to specify the path settings.');
    end
end

