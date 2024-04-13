from ctypes import alignment
import pandas as pd
import openpyxl
import os

# Function to manage data
def Manage_data(path_chess_csv, writer, Player1_Name, Player2_Name):
    data = pd.read_csv(path_chess_csv, sep=",")

    split_path = path_chess_csv.split('/')
    sheet = split_path[len(split_path)-1].split('.')[0]

    # Mean number of moves to win
    P1MeanWinMove = round(data[data['Player1'] == 'Win']['Move'].mean(), 0)
    P2MeanWinMove = round(data[data['Player2'] == 'Win']['Move'].mean(), 0)

    # Max number of moves to win
    P1MaxWinMove = round(data[data['Player1'] == 'Win']['Move'].max(), 0)
    P2MaxWinMove = round(data[data['Player2'] == 'Win']['Move'].max(), 0)

    # Min number of moves to win
    P1MinWinMove = round(data[data['Player1'] == 'Win']['Move'].min(), 0)
    P2MinWinMove = round(data[data['Player2'] == 'Win']['Move'].min(), 0)

    Player1Data = data['Player1'].value_counts()
    Player2Data = data['Player2'].value_counts()
    TotalRow = data['Game'].count()

    try:
        Player1Win = Player1Data['Win']
        NumWinPlayer1 = round((Player1Win/TotalRow) * 100, 2)
    except KeyError:
        NumWinPlayer1 = 0
        pass

    try:
        Player2Win = Player2Data['Win']
        NumWinPlayer2 = round((Player2Win/TotalRow) * 100, 2)
    except KeyError:
        NumWinPlayer2 = 0
        pass

    try:
        Draw = Player1Data['Draw']
        NumDraw = round((Draw/TotalRow) * 100, 2)
    except KeyError:
        NumDraw = 0
        pass

    # Write data in excel sheets
    df1 = pd.DataFrame({'Game': data['Game'], Player1_Name: data['Player1'], Player2_Name : data['Player2'], 'Moves': data['Move']})

    df2 = pd.DataFrame([[str(NumWinPlayer1) + '%', str(NumDraw)+ '%', P1MaxWinMove, P1MeanWinMove, P1MinWinMove],
                        [str(NumWinPlayer2) + '%', str(NumDraw)+ '%', P2MaxWinMove, P2MeanWinMove, P2MinWinMove]],
                        [Player1_Name, Player2_Name], 
                        ['Win', 'Draw', '#Moves to Win | Max', '#Moves to win | Mean', '#Moves to Win | Min'])
    
    df1.to_excel(writer, sheet_name=sheet + ' Data', index=False)
    df2.to_excel(writer, sheet_name=sheet + ' Statistics',  index=True)

    SetStyle(writer.book, sheet)
    

# Manage the graphics of the excel sheet
def SetStyle(excel_file, sheet):
    ws_data = excel_file[sheet + ' Data']
    ws_statistics = excel_file[sheet + ' Statistics']

    for sheet_name, ws in zip([sheet + ' Data', sheet + ' Statistics'], [ws_data, ws_statistics]):
        for col in ws.columns:
            max_length = 0
            column = col[0].column_letter
            for cell in col:
                try: 
                    if len(str(cell.value)) > max_length:
                        max_length = len(cell.value)
                except:
                    pass
            adjusted_width = (max_length + 2) * 1.2
            ws.column_dimensions[column].width = adjusted_width
        for row in ws.iter_rows():
            for cell in row:
                cell.alignment = openpyxl.styles.Alignment(horizontal='center', vertical='center')


path_array = ["../CSV/Human-Random.csv", "../CSV/Human-Minimax.csv", "../CSV/Random-Random.csv", "../CSV/Random-Minimax.csv", "../CSV/Minimax-Minimax.csv"]

script_dir = os.path.abspath(__file__)
parent_dir = os.path.dirname(script_dir)

with pd.ExcelWriter('Statistics.xlsx', engine='openpyxl') as writer:
    for path in path_array:

        # Extract the player name from the path
        words = path.split('-')
        Player1_Name = words[0].split('/')[2] + '(White)'
        Player2_Name = words[-1].split('.')[0] + '(Balck)'

        # Get the absolute path of the csv file
        csv_path = os.path.join(parent_dir, path)
        Manage_data(csv_path, writer, Player1_Name, Player2_Name)

