import cv2
import pytesseract
import chess
import chess.pgn

def extract_text_from_image(image_path):
    # Load the image using OpenCV
    image = cv2.imread(image_path)

    # Convert the image to grayscale for better OCR performance
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

    # Use Tesseract to extract text from the image
    text = pytesseract.image_to_string(gray)

    return text

def parse_moves(text):
    # Extract the moves from the text
    # Assuming the moves are listed in a standard format
    lines = text.split("\n")
    moves = []

    for line in lines:
        parts = line.split()
        # Only consider lines that have moves (e.g., "1. e4 e5")
        if len(parts) >= 3:
            moves.append(parts[1])  # White's move
            moves.append(parts[2])  # Black's move

    return moves

def generate_fen_from_moves(moves):
    # Create a new chess board
    board = chess.Board()

    for move in moves:
        try:
            board.push_san(move)  # Play the move on the board
        except ValueError:
            print(f"Invalid move: {move}")

    # Return the FEN string of the final position
    return board.fen()

if __name__ == "__main__":
    # Path to the screenshot image
    image_path = "C:/Users/Jonatan/Pictures/Screenshots/Screenshot 2024-12-27 180418.png"

    # Step 1: Extract text from the image
    extracted_text = extract_text_from_image(image_path)
    print("Extracted Text:", extracted_text)

    # Step 2: Parse the moves from the text
    moves = parse_moves(extracted_text)
    print("Extracted Moves:", moves)

    # Step 3: Generate the FEN string
    fen = generate_fen_from_moves(moves)
    print("Final FEN:", fen)